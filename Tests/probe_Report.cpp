#include "probe_Report.h"

#include "CtrlrPanel.h"
#include "CtrlrModulator.h"
#include "CtrlrLuaManager.h"
#include "Methods/CtrlrLuaMethodManager.h"
#include "Methods/CtrlrLuaMethod.h"
#include "CtrlrIDs.h"
#include "CtrlrComponents/CtrlrComponent.h"
#include "CtrlrPanel/CtrlrPanelCanvas.h"

namespace
{
    // True when `ancestor` is `c` or is somewhere above it in the component tree. Used to let a
    // group and the controls it owns overlap without being reported.
    bool isSelfOrAncestorOf(const juce::Component* ancestor, const juce::Component* c)
    {
        for (const juce::Component* p = c; p != nullptr; p = p->getParentComponent())
            if (p == ancestor)
                return true;
        return false;
    }

    // The live component of a modulator, or nullptr if it was never realized.
    juce::Component* liveComponentOf(CtrlrModulator& mod)
    {
        return static_cast<juce::Component*>(mod.getComponent());
    }
}

namespace probeReport
{
    bool isEffectivelyVisible(const juce::Component* c, const juce::Component* canvas)
    {
        // Walks up to and including the canvas; anything above the canvas belongs to the
        // (absent) editor window and is not part of what the panel document controls.
        for (const juce::Component* p = c; p != nullptr; p = p->getParentComponent())
        {
            if (! p->isVisible())
                return false;
            if (p == canvas)
                break;
        }
        return true;
    }

    juce::var makeObject()
    {
        return juce::var(new juce::DynamicObject());
    }

    void setProp(juce::var& obj, const juce::Identifier& key, const juce::var& value)
    {
        if (auto* o = obj.getDynamicObject())
            o->setProperty(key, value);
    }

    juce::String midiToHex(const juce::MidiMessage& m)
    {
        return juce::String::toHexString(m.getRawData(), m.getRawDataSize(), 1).trim().toLowerCase();
    }

    juce::var modulatorSnapshot(CtrlrModulator& mod, CtrlrPanelCanvas* canvas)
    {
        juce::var v = makeObject();
        setProp(v, "name", mod.getName());
        setProp(v, "value", mod.getModulatorValue());
        setProp(v, "componentType", mod.getComponentType().toString());
        setProp(v, "text", mod.getModulatorText());

        // Tree-level facts come from the <component> child of the modulator tree, so they are
        // still reported when the component was never realized.
        const juce::ValueTree comp = mod.getObjectTree().getChildWithName(Ids::component);
        setProp(v, "visible", (bool) comp.getProperty(Ids::componentVisibility, true));
        setProp(v, "componentRectangle", comp.getProperty(Ids::componentRectangle, juce::var()).toString());
        setProp(v, "componentLayerUid", comp.getProperty(Ids::componentLayerUid, juce::var()).toString());

        // Live-component facts. `visible` above only reflects the tree property, so a Lua
        // comp:setVisible(false) -- which never writes the tree -- used to report visible: true
        // for a control that is plainly hidden. These two fields close that gap.
        if (juce::Component* live = liveComponentOf(mod))
        {
            setProp(v, "liveVisible", live->isVisible());
            setProp(v, "effectiveVisible", isEffectivelyVisible(live, canvas));

            if (canvas != nullptr && live->getParentComponent() != nullptr)
                setProp(v, "canvasRectangle",
                        canvas->getLocalArea(live->getParentComponent(), live->getBounds()).toString());
        }

        setProp(v, "vstIndex", mod.getVstIndex());
        setProp(v, "min", mod.getMinModulatorValue());
        setProp(v, "max", mod.getMaxModulatorValue());
        return v;
    }

    juce::var layers(CtrlrPanel& panel)
    {
        // Layers (used as pages) live at panelTree -> uiPanelEditor -> uiPanelCanvasLayer[].
        // Read from the tree, which is also what setLayerVisible writes.
        juce::Array<juce::var> out;
        const juce::ValueTree editor = panel.getObjectTree().getChildWithName(Ids::uiPanelEditor);
        for (int i = 0; i < editor.getNumChildren(); ++i)
        {
            const juce::ValueTree layer = editor.getChild(i);
            if (! layer.hasType(Ids::uiPanelCanvasLayer))
                continue;
            juce::var l = makeObject();
            setProp(l, "name", layer.getProperty(Ids::uiPanelCanvasLayerName, juce::var()).toString());
            setProp(l, "visible", (bool) layer.getProperty(Ids::uiPanelCanvasLayerVisibility, true));
            setProp(l, "uid", layer.getProperty(Ids::uiPanelCanvasLayerUid, juce::var()).toString());
            out.add(l);
        }
        return out;
    }

    juce::var overlaps(CtrlrPanel& panel)
    {
        juce::Array<juce::var> out;
        CtrlrPanelCanvas* canvas = panel.getCanvas();
        if (canvas == nullptr)
            return out;

        // Gather the effectively-visible components once, with their absolute bounds, so the
        // pairwise pass below is a plain rectangle test.
        struct Placed { CtrlrModulator* mod; juce::Component* comp; juce::Rectangle<int> bounds; };
        std::vector<Placed> placed;
        for (int i = 0; i < panel.getNumModulators(); ++i)
        {
            CtrlrModulator* mod = panel.getModulatorByIndex(i);
            if (mod == nullptr)
                continue;
            juce::Component* comp = liveComponentOf(*mod);
            if (comp == nullptr || comp->getParentComponent() == nullptr)
                continue;
            if (! isEffectivelyVisible(comp, canvas))
                continue;
            placed.push_back({ mod, comp,
                               canvas->getLocalArea(comp->getParentComponent(), comp->getBounds()) });
        }

        for (size_t i = 0; i < placed.size(); ++i)
        {
            for (size_t j = i + 1; j < placed.size(); ++j)
            {
                // A group legitimately contains its own controls; only report overlaps between
                // components that are not on the same branch of the tree.
                if (isSelfOrAncestorOf(placed[i].comp, placed[j].comp)
                    || isSelfOrAncestorOf(placed[j].comp, placed[i].comp))
                    continue;

                const juce::Rectangle<int> hit = placed[i].bounds.getIntersection(placed[j].bounds);
                if (hit.isEmpty())
                    continue;

                juce::var o = makeObject();
                setProp(o, "a", placed[i].mod->getName());
                setProp(o, "b", placed[j].mod->getName());
                setProp(o, "rect", hit.toString());
                out.add(o);
            }
        }
        return out;
    }

    juce::var finalState(CtrlrPanel& panel)
    {
        CtrlrPanelCanvas* canvas = panel.getCanvas();

        juce::Array<juce::var> mods;
        for (int i = 0; i < panel.getNumModulators(); ++i)
            if (CtrlrModulator* mod = panel.getModulatorByIndex(i))
                mods.add(modulatorSnapshot(*mod, canvas));

        juce::var v = makeObject();
        setProp(v, "modulators", mods);
        setProp(v, "layers", layers(panel));
        setProp(v, "overlaps", overlaps(panel));
        return v;
    }

    juce::var luaCompileErrors(CtrlrPanel& panel)
    {
        juce::Array<juce::var> errors;
        CtrlrLuaMethodManager& mgr = panel.getCtrlrLuaManager().getMethodManager();
        for (int i = 0; i < mgr.getNumMethods(); ++i)
        {
            if (CtrlrLuaMethod* method = mgr.getMethodByIndex(i))
            {
                if (! method->isValid())
                {
                    const juce::String err = method->getLastError().getText().trim();
                    errors.add("method '" + method->getName() + "': "
                               + (err.isEmpty() ? juce::String("compile error") : err));
                }
            }
        }
        return errors;
    }

    juce::String toJsonString(const juce::var& v)
    {
        return juce::JSON::toString(v, false); // multi-line / pretty
    }
}

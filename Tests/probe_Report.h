#pragma once

// JSON report helpers for CtrlrPanelProbe (see Doc/PanelProbe.md).
//
// The whole report is assembled as a juce::var (DynamicObject graph) and serialised with
// juce::JSON at the end. These free functions keep probe_Main.cpp readable and are shared by
// the per-step observation code.

#include <JuceHeader.h>

class CtrlrPanel;
class CtrlrModulator;
class CtrlrPanelCanvas;

namespace probeReport
{
    // Make an empty JSON object backed by a DynamicObject, ready for setProperty().
    juce::var makeObject();

    // Set a property on an object created by makeObject().
    void setProp(juce::var& obj, const juce::Identifier& key, const juce::var& value);

    // Visible in its own right *and* not sitting inside a hidden group, layer or canvas: the
    // finalState `effectiveVisible` field, also the guard the `mouse` action applies.
    bool isEffectivelyVisible(const juce::Component* c, const juce::Component* canvas);

    // Lowercase, space-separated hex of a MIDI message's raw bytes, e.g. "f0 42 30 f7".
    juce::String midiToHex(const juce::MidiMessage& m);

    // Snapshot one modulator: { name, value, componentType, text, visible, liveVisible,
    // effectiveVisible, vstIndex, min, max, componentRectangle, canvasRectangle,
    // componentLayerUid }.
    //
    // Three visibility fields, because they answer three different questions and only one of
    // them is the one a test usually means:
    //   visible          -- the componentVisibility tree property (what the panel document says)
    //   liveVisible      -- the live Component's isVisible(), which a Lua comp:setVisible(false)
    //                       changes without touching the tree
    //   effectiveVisible -- liveVisible AND every ancestor up to the canvas being visible, so it
    //                       folds in a hidden group or layer. Assert on this one.
    //
    // canvasRectangle is absolute (canvas-relative) bounds; componentRectangle is the tree's
    // parent-relative getBoundsInParent(), which for a grouped control is relative to the group.
    // Both liveVisible/effectiveVisible/canvasRectangle need the live component and are omitted
    // when it was never realized.
    juce::var modulatorSnapshot(CtrlrModulator& mod, CtrlrPanelCanvas* canvas);

    // Snapshot the panel's canvas layers (pages), read from the ValueTree: [{name, visible, uid}].
    juce::var layers(CtrlrPanel& panel);

    // Every pair of effectively-visible components whose canvasRectangles intersect, as
    // [{ a, b, rect }]. Ancestor/descendant pairs (a group and the controls it owns) are skipped
    // -- that containment is by construction, not a layout bug.
    //
    // This has to work off the live component tree: componentRectangle is parent-relative, so
    // intersecting the reported rectangles would silently compare a group-relative rectangle
    // against a canvas-relative one and both miss real overlaps and invent false ones.
    juce::var overlaps(CtrlrPanel& panel);

    // Snapshot every modulator + every layer on the panel, plus any overlapping components:
    // { modulators: [ ... ], layers: [ ... ], overlaps: [ ... ] }.
    juce::var finalState(CtrlrPanel& panel);

    // Walk the method manager; return an array of "method 'name': <error>" strings for every
    // method that failed to compile (isValid() == false).
    juce::var luaCompileErrors(CtrlrPanel& panel);

    // Pretty-printed JSON text for a var.
    juce::String toJsonString(const juce::var& v);
}

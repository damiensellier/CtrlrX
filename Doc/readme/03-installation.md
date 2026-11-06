[← 02 — Licensing](02-licensing.md) | [Main Index](../../README.md) | Next: [04 — Compilation Guide →](04-compiling.md)

---

# 03 — Installation Guide

> **TL;DR**
> - Official pre-built installers for Windows and macOS are signed, notarized, and fully executable.
> - Linux distributions use packed zip formats requiring manual deployment into system or user-space plugin directories.
> - GPG signature verification steps are provided to validate package integrity on Linux targets.

## Contents
- [Windows](#windows)
- [macOS](#macos)
- [Linux](#linux)
- [Linux Signature Verification (GPG)](#linux-signature-verification-gpg)

---

## Windows

CtrlrX for Windows is distributed as a **signed $\text{.exe}$ installer**.

1.  **Download** the latest $\text{CtrlrX-*.exe}$ installer from the Releases page.
2.  **Double-click** the downloaded file to run the installer.
3.  Follow the on-screen prompts. The installer will guide you through selecting the **type of plugin** (Standalone EXE, VST3, or AAX) you wish to install and will automatically place the files in the correct system directories.

---

## macOS

CtrlrX for macOS is distributed as a **notarized and codesigned $\text{.pkg}$ package installer**.

1.  **Download** the latest $\text{CtrlrX-*.pkg}$ installer from the Releases page.
2.  **Double-click** the downloaded file to launch the macOS installer.
3.  Follow the on-screen prompts. The installer will automatically handle system security checks and guide you through selecting the **type of plugin** (Standalone APP, VST3, AU, or AAX) you wish to install.

---

## Linux

CtrlrX for Linux is distributed as a **signed $\text{.zip}$ archive** containing the necessary plugin files and libraries. Linux installations typically require manually extracting the files to the correct host application's plugin directory.

1.  **Download** the $\text{CtrlrX-Linux.zip}$ archive and the corresponding $\text{.sig}$ file from the Releases page.
2.  **Extract** the contents of the $\text{.zip}$ file. This will create a folder named `CtrlrX-*-Linux-Signed`.
3.  **Locate Your Plugin Folder:** Determine the correct location for VST/VST3/AAX plugins used by your Digital Audio Workstation (DAW). Common user-level locations are:
    * **VST:** `~/.vst/`
    * **VST3:** `~/.vst3/`
4.  **Copy the Plugin:** Move the contents of the extracted folder (or the necessary plugin files themselves) into the correct plugin directory for your DAW.

---

#### Linux Signature Verification (GPG)
For security and integrity, you can verify the downloaded files using the detached **GPG signature (`.sig`)** file included in the release. This ensures the files have not been tampered with since they were signed.

##### Prerequisites

You must have **GnuPG** (often just `gpg`) installed on your system.

##### 1. Import the Public Signing Key

The files are signed with the **CtrlrX Project Key** (`BDD8B991FDD59BDC`). Fetch this key from a public keyserver:

```bash
gpg --keyserver hkps://keys.openpgp.org --recv-keys BDD8B991FDD59BDC
```

##### 2. Extract and Verify

Download the main `CtrlrX-X.X.X-Linux.zip` file, which contains both the binary (e.g., `CtrlrX`) and the signature file (e.g., `CtrlrX.sig`).

1.  Extract the archive:
    ```bash
    unzip CtrlrX-*.zip
    # Navigate to the folder containing the binary and .sig file
    ```

2.  Run the verification command:

    ```bash
    gpg --verify CtrlrX.sig CtrlrX
    ```

##### Expected Output

A successful verification will show the message **`Good signature from "CtrlrX Project (LINUX GPG Signing Key) <ctrlrx@ctrlrx.org>"`**:

```
gpg: Signature made Tue Nov 4 02:46:47 2025 UTC
gpg:                using RSA key 643698F0E75E...24F1BC96
gpg: Good signature from "CtrlrX Project (LINUX GPG Signing Key) <ctrlrx@ctrlrx.org>" [unknown]
```

---

[← 02 — Licensing](02-licensing.md) | [Main Index](../../README.md) | Next: [04 — Compilation Guide →](04-compiling.md)
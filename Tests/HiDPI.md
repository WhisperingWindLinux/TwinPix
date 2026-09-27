# Image scaling regression check

The original Windows/4K report did not record the visual symptom or OS scale.
The regression covered here is a stale Fit In View transform after the viewport
changes size: shrinking clips the image, and enlarging leaves it unnecessarily
small. This is not yet confirmed to be the cause of the original Windows report.

## Automated check

Build `ImageViewerTests.pro` in a separate directory using the same Qt kit as
TwinPix (`qmake ../ImageViewerTests.pro`, then `make`, `mingw32-make` or `nmake`
as appropriate for the kit). The tests create a 3840×2160 image pair, fit it in
several viewport sizes, check all edges remain visible, and verify switching
preserves the mapping. They also check that resizing preserves manual zoom and
that selecting Fit In View restores automatic fitting.

Run each scale in a fresh process. POSIX shell, from the build directory:

```sh
for scale in 1 1.25 1.5 1.75 2; do
    QT_QPA_PLATFORM=offscreen QT_SCALE_FACTOR=$scale ./ImageViewerTests || exit 1
done
```

Windows PowerShell (adjust executable path for the kit's output directory):

```powershell
$env:QT_QPA_PLATFORM = 'offscreen'
foreach ($scale in '1', '1.25', '1.5', '1.75', '2') {
    $env:QT_SCALE_FACTOR = $scale
    & .\release\ImageViewerTests.exe
    if ($LASTEXITCODE -ne 0) { throw "Failed at scale $scale" }
}
Remove-Item Env:QT_QPA_PLATFORM, Env:QT_SCALE_FACTOR
```

The test logs the effective viewport DPR. These runs simulate Qt scale factors;
they do not substitute for the Windows display/compositor check below.

### Local result (2026-09-27)

- macOS 14.8.7 arm64, Qt 6.11.1, offscreen platform.
- Before the fix, DPR 1.5: the 4K resize test failed because image edges were
  outside the viewport; all three manual-zoom cases also failed after returning
  to Fit In View and resizing.
- After the fix: 52 passed, 0 failed at each effective DPR 1, 1.25, 1.5, 1.75,
  and 2, including the additional selection-zoom case.
- Native Windows/4K validation has not been performed.

## Native Windows/4K check

1. Clear `QT_SCALE_FACTOR`, `QT_SCREEN_SCALE_FACTORS` and `QT_QPA_PLATFORM`
   overrides. Record Windows version, Qt version, TwinPix commit, monitor
   resolution, and the scale shown in Windows Display settings.
2. Prepare two 3840×2160 PNG screenshots with visible details at all four edges
   and the same pixel dimensions. Record filenames and pixel dimensions.
3. Start TwinPix and open the pair. Before zooming, capture the initial view:
   all four edges should fit, without stretching.
4. Press S repeatedly. The image should keep its position and scale. Capture
   both images if their appearance differs unexpectedly.
5. Shrink the window, maximize it, then restore it. While Fit In View is active,
   both images should continue to fit and use the available space. Repeat S.
6. Select Actual Size: one source pixel should occupy one physical display
   pixel. Zoom manually and resize: the chosen zoom should remain unchanged.
   Press F to restore automatic fitting, then resize again.
7. Repeat after choosing a second Windows scale (for example 100%, 150%, 200%
   where available), restarting TwinPix for each run. Also change scale with
   the application open while Fit In View is active and repeat steps 3–5.

For each failure record the scale, window/viewport size, action, whether it
affects the first or second image, and a screenshot showing the symptom
(clipping, stretching, unexpected size or movement). Native Windows results
must be recorded separately from the offscreen test results.

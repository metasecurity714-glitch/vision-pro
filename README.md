# ClassVision — Final browser build

A mobile-friendly, GitHub Pages-ready classroom attendance prototype.

## Included
- Local teacher account creation, sign-in and logout
- Student registration with registration number, roll number, full name and branch
- Front and rear camera selection
- Class + subject/session setup once per attendance session
- Live face-presence detection with a green bounding box
- Manual student selection and identity confirmation before marking attendance
- Blocks attendance unless exactly one face is detected
- Duplicate prevention for the same student, class, subject and date
- Student search, edit and delete
- Attendance history, undo, CSV export
- JSON backup and restore
- IndexedDB storage, responsive mobile UI

## Important: face detection is not face recognition
The camera detects whether one face is present. It does **not** identify a person or compare faces against a stored biometric template. The teacher must select the student and confirm their identity against a class list or student ID. The name shown beside the green box is the manually selected student's name, not an AI identification result. This design avoids automated misidentification and biometric face-template storage.

## Deploy on GitHub Pages
1. Download and extract this ZIP.
2. Upload `index.html`, `style.css`, `app.js`, and `README.md` to the repository root.
3. In GitHub, open **Settings → Pages**.
4. Select **Deploy from a branch**, choose `main` and `/ (root)`, then Save.
5. Open the published HTTPS Pages URL in Chrome.
6. Create your local account, register students, begin a session, and start the camera.

Camera access requires HTTPS (GitHub Pages provides HTTPS) or localhost. On Android Chrome, allow camera permission when prompted.

## First camera launch
The face-presence detector loads MediaPipe Tasks Vision and its model from public CDNs. Internet access is required for that first load. Camera video is processed in the browser; no camera frames are uploaded by this app.

## Data and account limitations
- All account, student and attendance data is stored locally in IndexedDB on the current browser/device.
- No server, cloud sync, multi-device sharing, or central administrator is included.
- Browser storage may be cleared by device/browser cleanup. Export JSON backups regularly.
- The local account gate and password hashing are suitable only for a prototype; they are not a replacement for secure server-side authentication or access control.
- This is not a certified institutional attendance system. Supervise use and verify every identity before marking attendance.

## Files
- `index.html` — application layout
- `style.css` — responsive dark UI
- `app.js` — IndexedDB, account, student, camera, attendance and export logic
- `README.md` — setup and limitations

# ClassVision — Face Recognition Build

A mobile-friendly, GitHub Pages-ready classroom attendance prototype.

## Features
- Local teacher account creation, sign-in and logout
- Student registration with registration number, roll number, name and branch
- Face-photo enrollment during registration; only a 128-value face descriptor is retained, not the original photo
- Live face detection and face-descriptor matching against enrolled students
- Green box with the suggested student's name; unknown or ambiguous faces are labelled Unverified
- Front/rear camera switching
- Class and subject setup once per attendance session
- Attendance confirmation is enabled only for a confident match to the selected student, with a teacher confirmation checkbox
- Duplicate prevention for the same student, class, subject and date
- Student search, edit and delete; attendance history, undo and CSV export
- JSON backup and restore; IndexedDB local storage

## Important privacy and accuracy limits
Face templates are biometric data. Use this only with appropriate school approval and informed permission from each student and, where required, their parent or guardian. Do not enrol anyone who has not agreed. Templates are stored in this browser's IndexedDB and are **not encrypted by this prototype**. Anyone with access to the browser profile may be able to access local app data. JSON backups also contain face templates, so protect and delete backup files carefully. Deleting a student or clearing workspace data removes their stored template from this app.

Face matching can produce false matches or fail to recognise a student. Lighting, camera quality, distance, angle and changes in appearance affect results. The displayed distance is a model comparison score, not a probability or proof of identity. An unverified or ambiguous face cannot be marked automatically. A teacher must still check the suggested identity; do not use this prototype as the sole basis for attendance, discipline or other important decisions.

## Deploy on GitHub Pages
1. Extract this ZIP.
2. Upload `index.html`, `style.css`, `app.js` and `README.md` to the root of a new GitHub repository.
3. Open **Settings → Pages**.
4. Choose **Deploy from a branch**, select `main` and `/(root)`, then Save.
5. Open the published HTTPS URL in Chrome.
6. Create the local account, register students with a clear face photo and permission confirmation, start a class session, then start the camera.

Camera access requires HTTPS (GitHub Pages provides HTTPS) or localhost. On Android Chrome, allow camera permission.

## First use and connectivity
The first face enrollment or camera start downloads the face-recognition library and model weights from jsDelivr. Internet access is required to load them. Face matching and camera processing happen in the browser; this app does not upload camera frames to a server.

## Storage and account limitations
- Account, student, face-template and attendance data are stored locally in IndexedDB on the current browser/device.
- There is no server, cloud sync, multi-device sharing or central administrator.
- Clearing browser data may erase records; export backups carefully and keep them private.
- The local account gate is a prototype, not institutional-grade authentication or access control.
- Test with consent and supervised volunteers before any real classroom use.

## Files
- `index.html` — application layout
- `style.css` — responsive dark UI
- `app.js` — local storage, face recognition, camera, attendance and export logic
- `README.md` — setup and limitations

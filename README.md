# ClassVision — Face Recognition Build

A browser-based classroom attendance prototype. It stores student records and face descriptors in IndexedDB on the current device/browser.

## Deploy to a new GitHub repository
1. Extract this ZIP.
2. Upload the four files (`index.html`, `style.css`, `app.js`, `README.md`) from this folder to the repository root.
3. In GitHub, open **Settings → Pages**.
4. Select **Deploy from a branch**, choose `main` and `/(root)`, then Save.
5. Open the GitHub Pages HTTPS URL in Chrome and allow camera permission.

## Face enrollment and scanning
- Register each student with a clear, front-facing, well-lit photo containing only that student.
- Older student records created before face enrollment must be edited and given a face photo.
- Start a class/session, start the camera, and keep one face centred in good lighting.
- Live detection uses the SSD MobileNet face detector to better detect additional faces in the camera view.
- Attendance is saved automatically only after the same student is the sole detected face and produces a sufficiently close, distinct match across five consecutive scans.
- If zero faces, multiple faces, or an uncertain match is detected, attendance is paused. No student is selected manually and there is no separate Step 02 confirmation screen.
- Automatic biometric matching can still make mistakes. A teacher should review the attendance history against the class list before treating it as an official record.

## Important limitations and privacy
- This is an experimental, single-photo face-matching prototype, not a production biometric attendance system. Five-frame consistency and multi-face rejection reduce some errors but cannot guarantee identity. Recognition varies with lighting, camera angle, distance, image quality, and changes in appearance. Test with consent and supervised review before classroom use.
- Get appropriate school approval and informed permission from students and, where required, parents/guardians before collecting face templates. Provide a non-biometric alternative.
- Face descriptors are biometric data stored in this browser's IndexedDB. This prototype does not encrypt those descriptors or sync them to a secure school server. Do not use it for sensitive official records.
- The face recognition library and model files load from jsDelivr, so an internet connection is needed when they are first loaded.
- Camera access requires HTTPS (GitHub Pages provides HTTPS) and browser permission.
- Local login is only a prototype access gate, not secure institutional authentication.

## Troubleshooting
- If the camera starts but says **Unverified — no enrolled face**, edit each student and add a new face photo.
- If it says **Unverified — closest: ...**, capture a sharper, front-facing enrollment photo in even lighting, then test with the student facing the camera at a similar distance.
- If it says **Could not load face recognition library/models**, check internet access and reload the HTTPS site.
- If you updated files but the old behavior remains, hard-refresh Chrome or clear the site's cached files (do not clear site data unless you have exported a backup; that can delete local student data).


### Automatic identity selection update
Attendance scanning now automatically selects the matched student from the face template. The teacher no longer chooses a student from a dropdown. The app displays the matched student details and still requires the teacher to verify the identity before confirming attendance. Unknown or ambiguous faces remain unverified.

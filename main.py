import cv2
import numpy as np
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db

# Fetch the service account key JSON file contents
cred = credentials.Certificate('freemason2-3aaea-firebase-adminsdk-mi7n9-ddb44b02e1.json')

# Initialize the app with a service account, granting admin privileges
firebase_admin.initialize_app(cred, {
    'databaseURL': "https://freemason2-3aaea-default-rtdb.firebaseio.com/LED1/val"
})
ref = db.reference('/facedetect/val')

# Load the pre-trained face detection model
net = cv2.dnn.readNetFromDarknet('deploy.prototxt.txt', 'res10_300x300_ssd_iter_140000.caffemodel')

detector = cv2.CascadeClassifier('haarcascade_frontalface_default.xml')
cap = cv2.VideoCapture(0)

while (True):
    ret, img = cap.read()

    # Convert the image to a blob for the deep learning model
    blob = cv2.dnn.blobFromImage(img, 1.0, (300, 300), (104.0, 177.0, 123.0))

    # Set the input for the deep learning model
    net.setInput(blob)

    # Run the model to detect faces
    detections = net.forward()

    # Process the detected faces
    for i in range(detections.shape[2]):
        confidence = detections[0, 0, i, 2]

        # Check if the face is detected with high confidence
        if confidence > 0.5:
            x, y, w, h = detections[0, 0, i, 3:7] * np.array([img.shape[1], img.shape[0], img.shape[1], img.shape[0]])
            cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 2)
            ref.set(data)

    cv2.imshow('frame', img)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()
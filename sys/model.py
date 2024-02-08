import os
import cv2
import numpy as np
from skimage.feature import local_binary_pattern, hog
from sklearn.model_selection import train_test_split
from sklearn.svm import SVC
from sklearn.metrics import classification_report
from sklearn.preprocessing import StandardScaler

def load_images_from_folder(folder):
    images = []
    for filename in os.listdir(folder):
        img = cv2.imread(os.path.join(folder, filename), cv2.IMREAD_GRAYSCALE)
        if img is not None:
            images.append(img)
    return images

def otsu_thresholding(image):

    _, img_bin = cv2.threshold(image, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)
    return img_bin

def extract_features(images):
    features = []
    for img in images:
        img_bin = otsu_thresholding(img)
        lbp = local_binary_pattern(img_bin, P=24, R=3, method="uniform")
        lbp_hist, _ = np.histogram(lbp, density=True, bins=np.arange(0, 27), range=(0, 26))
        
        hog_feat = hog(img_bin, orientations=9, pixels_per_cell=(8, 8), cells_per_block=(2, 2), block_norm='L2-Hys')
        
        features.append(np.hstack([lbp_hist, hog_feat]))
    return features


pos_images = load_images_from_folder('path_to_positive_samples')
neg_images = load_images_from_folder('path_to_negative_samples')


labels = np.hstack([np.ones(len(pos_images)), np.zeros(len(neg_images))])


pos_features = extract_features(pos_images)
neg_features = extract_features(neg_images)
features = np.vstack([pos_features, neg_features])


scaler = StandardScaler()
features_scaled = scaler.fit_transform(features)


X_train, X_test, y_train, y_test = train_test_split(features_scaled, labels, test_size=0.2, random_state=42)


svm = SVC(kernel='linear')
svm.fit(X_train, y_train)


y_pred = svm.predict(X_test)
print(classification_report(y_test, y_pred))

import numpy as np
from sklearn import svm
from sklearn.metrics import accuracy_score, precision_score, recall_score
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt


# features = ... 
# labels = ... 


X_train, X_test, y_train, y_test = train_test_split(features, labels, test_size=0.2, random_state=42)


negative_sample_rates = np.linspace(0, 1, 11)[1:]


accuracies = []
precisions = []
recalls = []


positive_indices = np.where(y_train == 1)[0]
negative_indices = np.where(y_train == 0)[0]


for rate in negative_sample_rates:

    negative_count = min(int(len(negative_indices) * rate), len(negative_indices))
    positive_count = min(len(positive_indices) - negative_count, len(positive_indices))


    if positive_count < len(positive_indices) * (1 - rate):
        print(f"Skipping rate {rate:.2f} due to insufficient positive samples.")
        continue


    selected_positive = np.random.choice(positive_indices, positive_count, replace=False)
    selected_negative = np.random.choice(negative_indices, negative_count, replace=False)

    selected_indices = np.concatenate([selected_positive, selected_negative])
    np.random.shuffle(selected_indices)

    X_train_sub = X_train[selected_indices]
    y_train_sub = y_train[selected_indices]


    clf = svm.SVC(kernel='linear', class_weight='balanced') 
    clf.fit(X_train_sub, y_train_sub)


    y_pred = clf.predict(X_test)


    accuracies.append(accuracy_score(y_test, y_pred))
    precisions.append(precision_score(y_test, y_pred))
    recalls.append(recall_score(y_test, y_pred))


plt.plot(negative_sample_rates, accuracies, label='Accuracy')
plt.plot(negative_sample_rates, precisions, label='Precision')
plt.plot(negative_sample_rates, recalls, label='Recall')
plt.xlabel('Negative Sample Rate')
plt.ylabel('Performance Metric')
plt.title('SVM Performance at Different Negative Sample Rates')
plt.legend()
plt.show()

from sklearn.feature_extraction.text import CountVectorizer
from sklearn.naive_bayes import MultinomialNB
import joblib

class SimpleChatbot:
    def __init__(self):
        self.vectorizer = CountVectorizer()
        self.classifier = MultinomialNB()

    def train(self, inputs, outputs, model_path):
        X = self.vectorizer.fit_transform(inputs)

        self.classifier.fit(X, outputs)

        joblib.dump((self.vectorizer, self.classifier), model_path)

    def load_model(self, model_path):
        self.vectorizer, self.classifier = joblib.load(model_path)

    def predict(self, input_text):
        input_vec = self.vectorizer.transform([input_text])
        output = self.classifier.predict(input_vec)[0]
        
        return output

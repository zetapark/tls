from flask import Flask, render_template, request
import requests, json, time, re

app = Flask(__name__)

@app.route('/')
def index():
    return render_template('Bikin/index.html')

if __name__ == '__main__': app.run(port=5001, debug=True)

from flask import Flask
from flask import jsonify, request, make_response
from pywordcomplete import WordComplete
from flask_cors import CORS, cross_origin
import os
import gc
import argparse

app = Flask(__name__)

parser = argparse.ArgumentParser()
parser.add_argument("--compress", type=int)
args = parser.parse_args()

LOAD_COMPRESSED = args.compress

app.config['JSON_AS_ASCII'] = False
cors = CORS(app)
app.config['CORS_HEADERS'] = 'Content-Type'

CORPUS_PATH = './resources/VNESEcorpus.txt'
LIMIT_LINE = 10000

try:
    os.mkdir('./resources/trie')
except Exception:
    pass

word_complete = WordComplete()
if LOAD_COMPRESSED == 1:
    if not word_complete.load_compressed_data():
        word_complete.load_raw_data(CORPUS_PATH, LIMIT_LINE)
        word_complete.compress_data()
        word_complete.save_compressed_data()
else:
    word_complete.load_raw_data(CORPUS_PATH, LIMIT_LINE)
    word_complete.compress_data()
    word_complete.save_compressed_data()


@app.route("/api/complete", methods=['GET'])
@cross_origin()
def do_word_complete():
    import psutil
    process = psutil.Process(os.getpid())
    memory_usage = process.memory_info().rss / 1000000
    query = request.args.get('query', default='', type=str)
    candidates = word_complete.get_suggestion(query)
    result = {'candidates': candidates,
              'memory_usage': memory_usage}
    return make_response(jsonify(result))


@app.route("/api/load_compressed")
def load_compressed():
    global word_complete
    del word_complete
    gc.collect()
    word_complete = WordComplete()
    word_complete.load_compressed_data()
    return make_response(jsonify({'status': 200}))


@app.route("/api/load_raw")
def load_raw():
    global word_complete
    del word_complete
    word_complete = WordComplete()
    word_complete.load_raw_data(CORPUS_PATH, LIMIT_LINE)
    return make_response(jsonify({'status': 200}))


@app.route("/api/compress_raw")
def compress_raw():
    global word_complete
    word_complete.compress_data()
    word_complete.save_compressed_data()
    return make_response(jsonify({'status': 200}))


@app.route("/api/ping")
def ping():
    return "pong"


if __name__ == "__main__":
    # Only for debugging while developing
    app.run(host='0.0.0.0', port=6699)

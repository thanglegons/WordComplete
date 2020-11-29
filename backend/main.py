from flask import Flask
from flask import jsonify, request, make_response
from pywordcomplete import WordComplete
from flask_cors import CORS, cross_origin
import os
app = Flask(__name__)

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
if not word_complete.load_compressed_data():
    word_complete.load_raw_data(CORPUS_PATH, LIMIT_LINE)
    word_complete.compress_data()
    word_complete.save_compressed_data()


@app.route("/api/complete", methods=['GET'])
@cross_origin()
def do_word_complete():
    query = request.args.get('query', default='', type=str)
    candidates = word_complete.get_suggestion(query)
    result = {'candidates': candidates}
    return make_response(jsonify(result))


@app.route("/api/ping")
def ping():
    return "pong"


if __name__ == "__main__":
    # Only for debugging while developing
    app.run(host='0.0.0.0', port=6699)

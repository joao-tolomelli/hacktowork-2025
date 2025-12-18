from flask import Flask, render_template
import json
from redis import Redis

app = Flask(__name__)
r = Redis(host='localhost', port=6379)

@app.get('/')
def home():
    messages = r.lrange('mqtt_messages:chuveiro', 0, -1)
    return render_template('index.html', messages=messages)

@app.get('/api/history/chuveiro')
def history():
    data = r.lrange('mqtt_messages:chuveiro', 0, -1)
    parsed = [json.loads(v) for v in data]
    labels = ("chuveiro",) * len(data)  # o correto seria uma lista de timestamps
    vazao = [p['water'] / p['delta'] for p in parsed]
    res = {'labels': labels, 'values': vazao}
    print(res)
    return res

if __name__ == '__main__':
    app.run(debug=True)

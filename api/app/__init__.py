import eventlet
eventlet.monkey_patch()
from redis import Redis

import threading
from flask import Flask
from flask_cors import CORS
from flask_socketio import SocketIO

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})
socketio = SocketIO(app, cors_allowed_origins="*")
r = Redis(host='localhost', port=6379)

from .controllers.banho import bp as banho_bp
from .listener import redis_listener

app.register_blueprint(banho_bp)

@socketio.on('connect')
def handle_connect():
    print('Client connected')

@socketio.on('disconnect')
def handle_disconnect():
    print('Client disconnected')

def main():
    thread = threading.Thread(target=redis_listener)
    thread.daemon = True
    print("Starting Redis listener thread...")
    thread.start()
    print("Starting Flask-SocketIO server...")
    socketio.run(app)

if __name__ == '__main__':
    main()

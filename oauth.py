#!/usr/bin/python3

from google.oauth2 import id_token
from google.auth.transport import requests
import sys

idinfo = id_token.verify_oauth2_token(sys.argv[1], requests.Request(), '727978600466-bhdht9qmei6fkrap60or1hud2j7ahus4.apps.googleusercontent.com')

if idinfo['iss'] in ['accounts.google.com', 'https://accounts.google.com']:
    print(idinfo['email'])

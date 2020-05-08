#!/usr/bin/python3

from tls_crypt import Server, WebSite

class MySite(WebSite):
    def process(self):
        print(self.requested_document_)

f = MySite()
f.init('www')
sv = Server(3003)
sv.start(f)

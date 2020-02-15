 
openssl x509 -pubkey -in ../fullchain.pem -noout > pubkey.pem
 
openssl dgst -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-1 -sign ../privkey.pem -out test.sig test.txt
 
openssl dgst -sha256 -sigopt rsa_padding_mode:pss -sigopt rsa_pss_saltlen:-1 -signature test.sig -verify pubkey.pem test.txt
 
openssl base64 -in test.sig -out test.b64 -nopad

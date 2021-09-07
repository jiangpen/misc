from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec

with open("pk_verify.c", 'rb') as f1:
   msg=f1.read()
   
   
# Generate keypair
private_key = ec.generate_private_key(curve=ec.SECP256R1, backend=default_backend())
public_key = private_key.public_key().public_bytes(
      encoding=serialization.Encoding.PEM,
      format=serialization.PublicFormat.SubjectPublicKeyInfo,
 )
print(public_key)
prv_key=private_key.private_bytes(serialization.Encoding.PEM, format=serialization.PrivateFormat.TraditionalOpenSSL,encryption_algorithm=serialization.NoEncryption())
print(prv_key)
# Generate signature
sign=private_key.sign(msg, ec.ECDSA(hashes.SHA256()))
print(sign)
with open("prv.key", 'wb') as f1:
    f1.write(prv_key) 
with open("pub.key", 'wb') as f3:
    f3.write(public_key) 
with open("pk_verify.c.sig", 'wb') as f2:
    f2.write(sign) 

import hashlib
from ecdsa import NIST256p
from ecdsa import SigningKey
import binascii
import array
import sys

if sys.version_info[0] == 2:
	py2 = True
else:
	py2 = False

fp = open("key_info.txt","w")
sk = SigningKey.generate(curve=NIST256p)
print ("Private key: ",binascii.hexlify(sk.to_string()))
if py2:
	open("private.pem","w").write(sk.to_pem())
else:
	open("private.pem","wb").write(sk.to_pem())
	
fp.write("Private key: %s\n" % binascii.hexlify(sk.to_string()))

vk = sk.get_verifying_key()
if py2:
	open("public.pem","w").write(vk.to_pem())
else:
	open("public.pem","wb").write(vk.to_pem())
	
print (" Public Key " , binascii.hexlify(vk.to_string()))
fp.write("Public Key %s\n" % binascii.hexlify(vk.to_string()))

signerHash = hashlib.sha256()
for c in vk.to_string():
	if py2:
		signerHash.update(c.encode('hex')) 
	else:
		signerHash.update(c.to_bytes(1, 'little'))
L = list(signerHash.hexdigest())
signerInfo =""
for x in range(48,64):
	signerInfo+=L[x]
	
s = vk.to_string()

print ("Public key X : ", binascii.hexlify(s[:32]))
text = str(binascii.hexlify(s[:32]))
print(type(text))
# pad with zeros for even digits
text = text.zfill(len(text) + len(text) % 2)
print(text)
text = ' '.join(text[i: i+2] for i in range(1, len(text), 2))  # split into 2-digit chunks
#fp.write("Public key X: %s\n" % binascii.hexlify(s[:32]))
fp.write("Public key X: %s\n" % text)
print(text)


print ("Public key Y: ", binascii.hexlify(s[32:]))
text = str(binascii.hexlify(s[32:]))
# pad with zeros for even digits
text = text.zfill(len(text) + len(text) % 2)
text = ' '.join(text[i: i+2] for i in range(1, len(text), 2))  # split into 2-digit chunks
#fp.write("Public key Y: %s\n" % binascii.hexlify(s[32:]))
fp.write("Public key Y : %s\n" % text)
print(text)


s1 = binascii.hexlify(s[:32])
a=array.array('H',s1)
a.reverse()
print ("Public key X in reversed format : ", a.tostring())
fp.write("Public key X in reversed format : %s\n" % a.tostring())
s2 =  binascii.hexlify(s[32:])
b=array.array('H',s2)
b.reverse()
print ("Public key Y in reversed format: ", b.tostring())
fp.write("Public key Y in reversed format: %s\n" % b.tostring())

print ("Signer Info:" , signerInfo)
# pad with zeros for even digits
print(type(signerInfo))
text = text.zfill(len(signerInfo) + len(signerInfo) % 2)
text = ' '.join(signerInfo[i: i+2] for i in range(0, len(signerInfo), 2))  # split into 2-digit chunks
fp.write("Signer Info: %s" % text)
print(text)
#fp.write("Signer Info: %s" % text)
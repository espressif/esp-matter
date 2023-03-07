"""
/******************************************************************************
 @file	signUtil.py

 @brief This tool is used as a helper file for OAD Image Tool

 Group: WCS, BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2017 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/
"""
import sys
import struct
import hashlib
import imgBinUtil as util
from time import gmtime, strftime
import time
import os
import calendar
import computeCRC32
import binascii
from ecdsa import SigningKey
from ecdsa import VerifyingKey
from ecdsa import BadSignatureError

if sys.version_info[0] == 2:
	py2 = True
else:
	py2 = False

debug = False
signFile = "sign.bin"


def getHashStr(fileName):
	"""
	Function returns string to be hashed for the security verification the given binary image file.
	"""
	if py2:
		hashString = ''
	else :
		hashString = bytearray()

	with open(fileName, 'r+b') as filePtr:

		filePtr.seek(util.BIN_LEN_PTR, 0)
		binLen = int.from_bytes(filePtr.read(util.IMG_LEN_SIZE), byteorder='little')

		filePtr.seek(util.CRC_DATA_START_ADDR, 0)
		hashString += filePtr.read(4)

		filePtr.seek(util.FIXED_IMAGE_TYPE_ADDR, 0)

		# Reads in the remaining bytes in the fixed image header
		hashString += filePtr.read(util.FIXED_HDR_LEN - filePtr.tell())

		# Adds each segment to the hash string
		currSeg = util.FIXED_HDR_LEN
		while (currSeg < binLen):
			# Move the file pointer to the start of the segment
			filePtr.seek(currSeg, 0)
            # For the security segment we must skip the signature
			if (ord(filePtr.read(1)) == util.SEG_TYPE_SECURITY):
				segLen = util.getSegLen(fileName, currSeg)
				filePtr.seek(currSeg, 0)
				hashString += filePtr.read(segLen - util.ECDSA_SIGNATURE_LEN)
			else:
				segLen = util.getSegLen(fileName, currSeg)
				filePtr.seek(currSeg, 0)
				hashString += filePtr.read(segLen)
	        # Move the file pointer to the next segment
			currSeg += segLen

		if debug:
			print("len(hashString)", len(hashString))
	return hashString


def getSignInfo(privKeyFile):
	"""
	Function returns computes and returns the signerInfo from the given private key file .
	"""
	with open(privKeyFile, 'r') as filePtr:
		sk = SigningKey.from_pem(filePtr.read())

	vk = sk.get_verifying_key()

	signerHash = hashlib.sha256()
	if py2:
		for c in vk.to_string():
			signerHash.update(c.encode('hex'))
	else:
		signerHash.update(vk.to_string())

	L = list(signerHash.hexdigest())

	signerInfo =""
	for x in range(48,64):
		signerInfo+=L[x]
	if debug:
		print ("Signer Info:" , signerInfo)

	if py2:
		signerInfo = binascii.unhexlify(signerInfo)
	else:
		signerInfo  = binascii.unhexlify(signerInfo)

	if debug:
		hashFile="hash.bin"
		with open(hashFile, 'wb+') as filePtr:
			filePtr.write(signerInfo)
	return signerInfo


def signImage(fileName, privKeyFile):
	"""
	Function returns sign the given binary with private key file.
	"""

	imgLen = -1

	if debug:
		print("sign_image :: FileName:", fileName)
		print("sign_image :: privKeyFile:", privKeyFile)

	msgFile = fileName

	updateTime(fileName)

	# Update the signer info
	signerInfo = getSignInfo(privKeyFile)
	secureSegAddr = util.getSegAddr(msgFile, util.SEG_TYPE_SECURITY)
	with open(msgFile, 'r+b') as filePtr:
		if secureSegAddr != -1:
			filePtr.seek(secureSegAddr, 0)
			filePtr.seek(util.SIGNATURE_PAYLOAD_OFFSET, 1)
			securityVer = ord(filePtr.read(1))

			# Handles ECDSA P-256 signature
			# No need to do security version check, this is for supporting 
			# anti-rollback protection feature in dual image on-chip BIM			
			# Creating the signer info
			signerAddr = secureSegAddr + util.SIGN_PAYLOAD_OFFSET
			filePtr.seek(signerAddr, 0)
			filePtr.write(signerInfo)			
		else:
			raise Exception('No security segment found')

	# Create the string that will be used to create the signature
	message = getHashStr(msgFile)

	# Get the private key
	with open(privKeyFile, 'r') as filePtr:
		sk = SigningKey.from_pem(filePtr.read())
	# Create the signature
	sig = sk.sign(data=message, hashfunc=hashlib.sha256)

	with open(signFile, 'w+b') as filePtr:
		filePtr.write(sig)

	# Write the signature to the bin file
	signatureAddr = secureSegAddr + util.SIGN_PAYLOAD_OFFSET + util.ECDSA_SIGNER_LEN
	with open(msgFile, 'rb+') as filePtr:
		signAddr = secureSegAddr + util.SIGN_PAYLOAD_OFFSET + util.ECDSA_SIGNER_LEN
		filePtr.seek(signAddr, 0)
		filePtr.write(sig)

	# Update CRC
	computeCRC32.computeCRC32(msgFile)


	if debug:
		crc = computeCRC32.crc32_withOffset(msgFile, util.CRC_DATA_START_ADDR)
		print("crc - With regular compute: ", crc)

	if debug:
		pubKey = os.path.join(os.path.dirname(__file__), 'public.pem')
		#Load the verifying key, message, and signature from disk, and verify the signature:
		with open(pubKey, 'r') as filePtr:
			vk = VerifyingKey.from_pem(filePtr.read())

		message = getHashStr(msgFile)
		with open(signFile, 'rb') as filePtr:
			sig = filePtr.read()
		try:
			vk.verify(sig, message, hashfunc=hashlib.sha256)
			print ("GOOD SIGNATURE")
		except BadSignatureError:
			raise Exception("BAD SIGNATURE")

def updateTime(fileName):
	'''
	Function updates the time-stamp part of the security segment information.
	'''

	# open file in binary mode
	with open(fileName, 'rb+') as filePtr:

		# Read the image end address
		filePtr.seek(util.IMG_END_ADDR_PTR, 0)
		addrTmp = (filePtr.read(4))
		imgEndAddr = util.computeAddr(addrTmp)

		# Read the binary file's length
		filePtr.seek(util.BIN_LEN_PTR, 0)
		addrTmp = (filePtr.read(4))
		imgLen = util.computeAddr(addrTmp)

		# Move to start of security segment
		imgSecStartAddr = util.getSegAddr(fileName, util.SEG_TYPE_SECURITY)

	# Gets the system time and writes it to the next four bytes
	timeStamp = strftime("%Y-%m-%d %H:%M:%S", gmtime())
	hexTime = hex(int(calendar.timegm(time.strptime(timeStamp,'%Y-%m-%d %H:%M:%S'))))
	hexInt = int(hexTime, 16)

	if debug:
		print(" Time ::: ", hexInt)

	util.writeBytes(fileName, (imgSecStartAddr+util.TIMESTAMP_OFFSET), hexInt, util.IMG_LEN_SIZE)


if __name__ == '__main__':
	sys.exit(main())
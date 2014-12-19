#
# DJango field type for a Bitcoin Address
#
from hashlib import sha256

digits58 = '123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz'
__b58base = len(digits58)

def decode_base58(bc, length):
    n = 0
    for char in bc:
        n = n * 58 + digits58.index(char)
    return n.to_bytes(length, 'big')

def encode_base58(bytestring):
    """Encode the bytestring to a base58 encode string. """
    # Count zero's
    zeros = 0
    for i in range(len(bytestring)):
        if bytestring[i] == 0:
            zeros += 1
        else:
            break
    try:
        n = int.from_bytes(bytestring, 'big')
    except AttributeError:
        # Python version < 3.2
        n = _bytes_to_long(bytestring, 'big')
    result = ''
    (n, rest) = divmod(n, 58)
    while n or rest:
        result += digits58[rest]
        (n, rest) = divmod(n, 58)
    #return result
    return zeros * '1' + result[::-1] # reverse string

def check_bc(bc):
    bcbytes = decode_base58(bc, 25)
    return bcbytes[-4:] == sha256(sha256(bcbytes[:-4]).digest()).digest()[:4]

def burnaddr(a):
  bcbytes = decode_base58(a, 25)
  b = sha256(sha256(bcbytes[:-4]).digest()).digest()[:4]
  b = bcbytes[:-4] + b
  result = encode_base58(b)
  if check_bc(result): print(result)
  else: raise('Bad addr')

if __name__ == '__main__':
  burnaddr('sShibesBurn3oMBuyNowBurnxx1xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnxx2xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnxx3xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnxx4xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnxx5xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnxx6xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnxx7xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnxx8xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnxx9xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx1oxxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx11xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx12xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx13xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx14xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx15xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx16xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx17xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx18xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx19xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx2oxxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx21xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx22xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx23xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx24xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx25xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx26xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx27xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx28xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx29xxxxxxx')
  burnaddr('sShibesBurn3oMBuyNowBurnx3oxxxxxxx')

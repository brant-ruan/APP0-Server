#!/usr/bin/env python

import qrcode

data = 'Hello, world'
img_file = "./QRCode/xxx.xxx.xxx.xxx_qr.png"

img = qrcode.make(data)
img.save(img_file)

img.show()

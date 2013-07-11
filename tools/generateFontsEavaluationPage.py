from PIL import Image, ImageFont, ImageDraw
import os
from xml.dom.minidom import Document

class FontTestWriter:
    'The font test suite writer'
    
    forbidden = ["ttf-japanese", "Kacst", "Kedage", "lohit", "Mallige", "mry", "msam", "OpenSymbol", "ori", "Pothana", "Saab", "Untitled", "Vemana", "wasy", "esint", "cmex", "cmr", "Balker", "msbm", "rsfs", "cmsy", "cmmi10" ]
    
    fonts = []
    capitals = []
    
    
 
    def __init__(self):
        
        #self.getFonts('/usr/share/fonts/truetype')
        #self.getFonts('/home/busta/data/cvut/textspotter/fonts')
        self.getFonts('C:/SkewDetection/fonts')
        
    
    def getFonts(self, startDir):
        for dirname, dirnames, filenames in os.walk(startDir):
            for filename in filenames:
                print (os.path.join(dirname, filename))
        
                bad = False
                for  f in self.forbidden:
                    if filename.startswith(f):
                        bad = True
                if bad:
                    continue
                
                font = ImageFont.truetype(os.path.join(dirname, filename), 64)
                for  f in self.forbidden:
                    if font.getname()[0].startswith(f):
                        bad = True
                if bad:
                    continue
                
                capitals = False
                if dirname.endswith("capitals"):
                    capitals = True
                if dirname.endswith("art"):
                    continue
                
                font = ImageFont.truetype(os.path.join(dirname, filename), 64)
                self.fonts.append(font)
                self.capitals.append(capitals)
                

    def writeSentence(self, text, outputDir, chrName, font, capital):
    
        im = Image.new("RGB", (20, 20))
        draw = ImageDraw.Draw(im)
    
        w, h = draw.textsize(text, font=font)
        im = Image.new("RGB", (w+30, h+40),  (255,)*3)
        draw = ImageDraw.Draw(im)
        draw.text((20, 20), text, font=font, fill="#000000")
  
        imageDir = outputDir
        
        if 'italic' in font.getname()[1].lower():
        	imageDir = imageDir + '/_italic/' + chrName
        else:
        	imageDir = imageDir + '/' + chrName
        
        if not os.path.exists(imageDir):
            os.mkdir( imageDir )
        
        imageName = font.getname()[0] + "-" + font.getname()[1] + ".png"
        
        imageName = imageName.replace("&", "")
        imageName = imageName.replace(" ", "")
        
        imageFile = imageDir + os.sep + imageName
        if im.size[0] < 2:
            return 
        if im.size[1] < 2:
            return 
            
        im2 = Image.new("RGB", (w+30, h+40),  (0,)*3)
        draw2 = ImageDraw.Draw(im2)
        draw2.fontmode = "1"
        draw2.text((0, 0), text, font=font, fill="#FFFFFF")
        bbox2 = im2.getbbox()
        if bbox2 == None:
            return
        else: 
        	if text != 0 or text != 'o' or text != 'O':
        		left, upper, right, lower = bbox2
        		#left = left + 1
        		#upper = upper + 1
        		right = right - 1
        		lower = lower - 1
        		pix = im2.load()
        		frame = True
        		for x in range(left, right): 
        			if pix[x, upper] != (255,)*3:
        				frame = False
        		for x in range(left, right): 
        			if pix[x, lower] != (255,)*3:
        				frame = False
        		for y in range(upper, lower): 
        			if pix[left, y] != (255,)*3:
        				frame = False
        		for y in range(upper, lower): 
        			if pix[right, y] != (255,)*3:
        				frame = False
        		
				hide = True
						
        		for count, color in im2.getcolors():
        			if color == (255,)*3 and count >= ((right-left)*(lower-upper)):
        				hide = False
        				
        		if frame and hide:
        			return
        		
        im.save(imageFile)
        
        '''imageTag = outDoc.createElement("image") SFAtarianSystem-Regular
        tagset.appendChild(imageTag)
        imageNameTag = outDoc.createElement("imageName")
        imageNameTag.appendChild(outDoc.createTextNode(imageName))
        imageTag.appendChild(imageNameTag)
        taggedRectangles = outDoc.createElement("taggedRectangles")
        imageTag.appendChild(taggedRectangles)
        split = text.split(" ")
        startx = 20
        starty = 20
        ws, hs = draw.textsize(" ", font=font)
        
        totals = "";
        for  s in split:
            w, h = draw.textsize(s, font=font)
            im2 = Image.new("RGB", (w+30, h+40),  (0,)*3)
            draw2 = ImageDraw.Draw(im2)
            draw2.text((0, 0), s, font=font, fill="#FFFFFF")
            bbox2 = im2.getbbox()
            if bbox2 == None:
                continue
            
            h = bbox2[3] - bbox2[1]
            w = bbox2[2] - bbox2[0]
            
            totals += s;
            w2, h2 = draw.textsize(totals, font=font)
            im2 = Image.new("RGB", (w2+30, h2+40),  (0,)*3)
            draw2 = ImageDraw.Draw(im2)
            draw2.text((20, 10), totals, font=font, fill="#FFFFFF")
            bbox3 = im2.getbbox()
            
            startx = bbox3[2] - w 
            
            taggedRectangle = outDoc.createElement("taggedRectangle")
            taggedRectangle.setAttribute("x", str(startx))
            taggedRectangle.setAttribute("y", str(starty + bbox2[1]))
            taggedRectangle.setAttribute("width", str(w))
            taggedRectangle.setAttribute("height", str(h))
            taggedRectangle.setAttribute("modelType", "1")
            
            tagg = outDoc.createElement("tag")
            nodeText = s
            if capital: 
                nodeText = nodeText.upper()
            tagg.appendChild(outDoc.createTextNode(nodeText))
            taggedRectangle.appendChild(tagg)
            
            startx = bbox3[2] + ws
            taggedRectangles.appendChild(taggedRectangle)
            totals += " ";'''
        
    def process(self, text, outputDir, chrName, allowed = False, specific_fonts = None):
        
        #outDoc = Document()
        #tagset = outDoc.createElement("tagset")
        #outDoc.appendChild(tagset)
        
        if not os.path.exists(outputDir):
            os.mkdir( outputDir )
        
        if not os.path.exists(outputDir+'/_italic'):
            os.mkdir( outputDir+'/_italic' )
        
        for i in range(len(self.fonts)):
            font = self.fonts[i]
            if specific_fonts != None:
                bad = allowed
                for  f in specific_fonts:
                    if font.getname()[0].startswith(f):
                        bad = not allowed
                if bad:
                    continue
            capital = self.capitals[i]
            self.writeSentence(text, outputDir, chrName, font, capital)
        
        #annotedFile = outputDir + os.sep + "gt.xml"
        #f = open(annotedFile, 'w')
    	#outDoc.writexml(f)
        

writer = FontTestWriter()

#DIGIT
for i in range(0x0030, 0x0039):
	writer.process(unichr(i), "C:/SkewDetection/out", str(i))

#LATIN CAPITAL
for i in range(0x0041, 0x005A):
	writer.process(unichr(i), "C:/SkewDetection/out", str(i))

#LATIN SMALL
specific_fonts = ['Twelve Ton Fishstick', '20.000 dollar bail', 'Airacobra Alt', 'Alan Den', 'Alexis Bold', 'Alien', 'All Caps', 'Amalgam', 'Babes In Toyland NF', 'BadaBoom BB', 'Cactus', 'Data Control', 'NeverSayDie', 'Postmaster', 'SF Atarian System', 'Super Ultra 911', 'Turntablz BB']
for i in range(0x0061, 0x007A):
	writer.process(unichr(i), "C:/SkewDetection/out", str(i), False, specific_fonts)

#GREEK
specific_fonts = ['Alpha Beta', 'Dark Side', 'El Wonko', 'Hack & Slash', 'Kinnari', 'Microsoft PhagsPa', 'Plantagenet Cherokee']
for i in range(0x0370, 0x03FF):
	writer.process(unichr(i), "C:/SkewDetection/out", str(i), False, specific_fonts)

#CYRIL
specific_fonts = ['Alpha Beta', 'Dark Side', 'El Wonko', 'Hack & Slash', 'Kinnari', 'Microsoft PhagsPa', 'Plantagenet Cherokee']
for i in range(0x0400, 0x04FF):
	writer.process(unichr(i), "C:/SkewDetection/out", str(i), False, specific_fonts)

#GEORGIAN
specific_fonts = ['Sylfaen', 'FreeSerif']
for i in range(0x10A0, 0x10FA):
	writer.process(unichr(i), "C:/SkewDetection/out", str(i), True, specific_fonts)      

#RUNIC
specific_fonts = ['Segoe UI Symbol']
for i in range(0x16A0, 0x16F0):
	writer.process(unichr(i), "C:/SkewDetection/out", str(i), True, specific_fonts)


'write modifications'


'''if not os.path.exists("/tmp/ocr/modifications"):
    os.mkdir( "/tmp/ocr/modifications", 0777 )
if not os.path.exists("/tmp/ocr/modifications/TextLenth"):
    os.mkdir( "/tmp/ocr/modifications/TextLenth", 0777 )
    
for i in range(4, len(text)):  
    writer.process(text[0:i], "C:/SkewDetection/{0}".format(i))'''
'''    
writer = FontTestWriter()  
for i in range(0, 9): 
'''	
#!/usr/bin/python

import sys


class Triangle():
  def __init__(self, items):
    self.items = items
    self.valid = items[0]
    self.verts = items[1]
    self.v0 = Vertice(items[2], items[3], items[4])
    self.v1 = Vertice(items[5], items[6], items[7])
    self.v2 = Vertice(items[8], items[9], items[10])
    self.v3 = Vertice(items[11], items[12], items[13])
    self.color = Vertice(items[14], items[15], items[16])

  def adjacent(self, other):
    retVal = self.valid and other.valid and \
           self.verts == "3" and other.verts == "3" and \
           self.color.eq(other.color)
    eqSides = 0
    if self.v0.eq(other.v0) or self.v0.eq(other.v1) or self.v0.eq(other.v2):
      eqSides += 1
    if self.v1.eq(other.v0) or self.v1.eq(other.v1) or self.v1.eq(other.v2):
      eqSides += 1
    if self.v2.eq(other.v0) or self.v2.eq(other.v1) or self.v2.eq(other.v2):
      eqSides += 1
    return retVal and (eqSides == 2)            

  def __str__(self):
    if not self.valid:
      return "invalid"
    retVal = "vertices: %s\n" % self.verts
    retVal += "\tv0: %s\n" % self.v0.__str__()
    retVal += "\tv1: %s\n" % self.v1.__str__()
    retVal += "\tv2: %s\n" % self.v2.__str__()
    retVal += "\tv3: %s\n" % self.v3.__str__()
    retVal += "\tcolor: %s\n" % self.color.__str__()
    return retVal 

  def write(self):
    return ' '.join(self.items)

  def writeQuad(self, other):
    retVal = '1 4 ' + self.v0.write() + ' ' + self.v1.write() + ' ' + self.v2.write() + ' '
    if not other.v0.eq(self.v0) and not other.v0.eq(self.v1) and not other.v0.eq(self.v2):
      retVal += other.v0.write()
    elif not other.v1.eq(self.v0) and not other.v1.eq(self.v1) and not other.v1.eq(self.v2):
      retVal += other.v1.write()
    elif not other.v2.eq(self.v0) and not other.v2.eq(self.v1) and not other.v2.eq(self.v2):
      retVal += other.v2.write()
    retVal += ' ' + self.color.write()
    return retVal

class Vertice():
  def __init__(self, x, y, z):
    self.x = x
    self.y = y
    self.z = z

  def eq(self, other):
    return self.x == other.x and self.y == other.y and self.z == other.z

  def __str__(self):
    return "(" + self.x + "," + self.y + "," + self.z + ")"

  def write(self):
    return ' '.join([self.x, self.y, self.z])

def main(oFile, iFile):

  out = open(oFile, 'w')

  tris = 0
  adj = 0

  lastTri = None
  for line in open(iFile):
    items = line.strip().split(' ')
    if len(items) != 17:
      out.write(line)
    else:
      tri = Triangle(items)
      tris += 1
      if lastTri and lastTri.adjacent(tri):
        adj += 1
        #print " **** "
        #print lastTri
        #print tri
        #print " **** "
        out.write(lastTri.writeQuad(tri) + '\n')
        lastTri = None
      elif tri.valid == "1":
        if lastTri:
          out.write(lastTri.write() + '\n')
        lastTri = tri

  print "processed %i triangles" % tris
  print "found %i adjacent triagles" % adj

if __name__ == '__main__':
  oFile = sys.argv[1]
  iFile = sys.argv[2]
  main(oFile, iFile)

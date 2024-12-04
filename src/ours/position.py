
# https://www.redblobgames.com/grids/hexagons/

# We use the cube coordinates (axial coordinates, so we don't store s)

class Position:
    
    q = None
    r = None
    h = 1
    # s = -q-r

    def __init__(self, q = None, r = None):
        self.q = q
        self.r = r

    def __str__(self) -> str:
        if (self.q != None):
            return "Position ({}, {}, {} | {})".format(self.q,self.r,-self.q-self.r,self.h)
        return "Not placed"

    def is_placed(self):
        return self.q != None

    def left(self):
        return Position(self.q - 1, self.r)
    
    def right(self):
        return Position(self.q + 1, self.r)
    
    def up_left(self):
        return Position(self.q, self.r - 1)
    
    def up_right(self):
        return Position(self.q + 1, self.r - 1)
    
    def down_left(self):
        return Position(self.q - 1, self.r + 1)
    
    def down_right(self):
        return Position(self.q, self.r + 1)
    
    def increase_height(self):
        self.h += 1

    def decrease_height(self):
        self.h -= 1

    def is_over(self, other):
        return other.r == self.r and other.q == self.q and other.h > self.h
    
    def is_near(self, other):
        return self.left() == other or self.right() == other or self.up_left() == other or self.up_right() == other or self.down_left() == other or self.down_right() == other
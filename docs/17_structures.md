# Structures

```
struct Point:
    var:int x, y

func:string pointToString(Point point):
    return "(" + point.x + ", " + point.y + ")"

func main():
    var point = Point(1, 1)
    >> pointToString(point)

```

```
struct Point:
    var:int x, y

func:string pointToString(Point point):
    return "(" + point.x + ", " + point.y + ")"

func:Point euclidean(Point p1, Point p2):
    var:Point point = Point(0, 0)
    sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2))
    return point

func main():
    var:Point p1 = {x=2, y=4}
    var p2 = (Point){x=3, y=5}
    var p3 = euclidean(p1, p2)
    >> pointToString(p3)

```

[Index](index.md) | [<< Back](16_enumerators.md)
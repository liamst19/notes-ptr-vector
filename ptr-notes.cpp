/** ptr-study.cpp
 *
 *  Notes on inheritance, polymorphism,
 *  and using vector of pointers.
 * 
 */

#include <stdio.h>
#include <iostream>
#include <vector>
#include <memory> // For std::unique_ptr and std::move


// Base abstract type class
class DrawingElement{
public:
  DrawingElement(){}
  ~DrawingElement(){}

  // All inherited classes must implement render()
  virtual void render()=0;
};

// Point
class Point: public DrawingElement{
public:
  Point(int x, int y): _x(x), _y(y){}

  ~Point(){}

  void render() override{
    std::cout << "Rendering a point (" << _x << ", " << _y << ")" << std::endl;
  }
  
private:
  int _x, _y;
};

// Line
class Line: public DrawingElement{
public:
  Line(int x1, int y1, int x2, int y2): 
    _x1(x1), _y1(y1), 
    _x2(x2), _y2(y2){}

  ~Line(){}

  void render() override{
    std::cout << "Rendering a line from (" << _x1 << ", " << _y1
              << ") to (" << _x2 << ", " << _y2 << ")" << std::endl;
  }
private:
  int _x1, _y1, _x2, _y2;
};

// Rectangle
class Rectangle: public DrawingElement{
public:
  Rectangle(int x, int y, int w, int h): 
    _x(x), _y(y),
    _w(w), _h(h){}

  ~Rectangle(){}

  void render() override{
    // Render rectangle
    std::cout << "Rendering a rectangle of dimension " << _w << "x" << _h
              << ", from (" << _x << ", " << _y << ")" << std::endl;
  }
private:
  int _x, _y, _w, _h;
};

// Drawing, which contains a collection of elements
// representing small parts that make up a single drawing
class Drawing{
public:
  Drawing(){}
  ~Drawing(){
    clear_drawing_ptrs();
  }

  // --------------------------------------------------
  /* !! This will throw an compilation error: 
     
     "cannot declare parameter 'element' to be of
      abstract type 'DrawingElement'"
  */

  // Add drawing element to collection
  // void add_element(DrawingElement element){
  //   _drawing.push_back(element);
  // }
  // --------------------------------------------------

  // Add drawing element pointer to collection
  void add_element_ptr(DrawingElement* element_ptr){
    _drawing_ptrs.push_back(element_ptr);
  }

  // Add drawing element unique pointer to collection
  void add_element_u_ptr(std::unique_ptr<DrawingElement> element_u_ptr){

    // This will not work, because vector tries
    // to *copy* a unique pointer.
    // _drawing_u_ptrs.emplace_back(elementUPtr);

    // Unique pointers must be moved
    _drawing_u_ptrs.emplace_back(std::move(element_u_ptr));
  }
  
  // --------------------------------------------------
  // This throws an compilation error
  //
  // Rendering from collection of DrawingElement objects
  // void render(){
  //   for(auto element: _drawing){
  //     element.render();
  //   }
  // }
  // --------------------------------------------------

  // Rendering from collection of pointers to DrawingElement
  void render_ptrs(){
    std::cout << std::endl;

    if(_drawing_ptrs.empty()){
      std::cout << "Collection of pointers is empty" << std::endl;
      return;
    }
    
    std::cout << "Rendering " << _drawing_ptrs.size() << " elements from pointers" << std::endl;
    for(auto element_ptr: _drawing_ptrs){
      if(element_ptr == nullptr){
        std::cout << "  ** Null pointer" << std::endl;
      } else{
        element_ptr->render();
      }
    }
  }

  // Rendering from collection of unique pointers to DrawingElement
  void render_u_ptrs(){
    std::cout << std::endl;
    std::cout << "Rendering " << _drawing_u_ptrs.size() << " elements from unique pointers" << std::endl;

    // Must dereference to be used
    for(auto& elementPtr: _drawing_u_ptrs){
      elementPtr->render();
    }
  }
  
  // Factory Methods ------------------------------
  
  static std::unique_ptr<Point> getPointPtr(int x, int y){
    return std::unique_ptr<Point>{new Point{x, y}};
  }
  
  static std::unique_ptr<Line> getLinePtr(int x1, int y1, int x2, int y2){
    return std::unique_ptr<Line>{new Line{x1, y1, x2, y2}};
  }
  
  static std::unique_ptr<Rectangle> getRectanglePtr(int x, int y, int w, int h){
    return std::unique_ptr<Rectangle>{new Rectangle{x, y, w, h}};
  }

  // -----------------------------------------------

  // Draw some basic elements
  void draw(){
    _drawing.clear();

    /* 
       The following will give compilation error:

         "invalid new-expression of abstract class type 'DrawingElement'"
    */
    // _drawing.push_back(Point{10, 15});
    // _drawing.push_back(Line{25, 25, 50, 100});
    // _drawing.push_back(Rectangle{50, 50, 100, 75}); 
    // --------------------------------------------------

    // We can create individual elements
    Point      point{10, 15};
    Line       line{25, 25, 50, 100};
    Rectangle  rect{50, 50, 100, 75};

    /*
       But we can't insert them to the vector:

         "invalid new-expression of abstract class type 'DrawingElement'"
    */
    // _drawing.push_back(point);
    // _drawing.push_back(line);
    // _drawing.push_back(rect);
    // --------------------------------------------------
  }

  void draw_ptrs(){
    _drawing_ptrs.clear();

    Point      point{10, 15};
    Line       line{25, 25, 50, 100};
    Rectangle  rect{50, 50, 100, 75};

    // Adding pointers work, but as soon as we leave this scope
    // the objects are destroyed. Calling render() from where
    // draw_ptrs() was called will throw an exception at runtime:
    //    "pure virtual method called"
    // _drawing_ptrs.push_back(&point);
    // _drawing_ptrs.push_back(&line);
    // _drawing_ptrs.push_back(&rect);
    // --------------------------------------------------

    // We can use the `new` keyword, but we must remember
    // to clear the allocated memory.
    _drawing_ptrs.push_back(new Point{45, 55});
    _drawing_ptrs.push_back(new Line{88, 98, 456, 987});
    _drawing_ptrs.push_back(new Rectangle{879, 654, 123, 321});
  }

  // Clear pointers created with `new` keyword
  void clear_drawing_ptrs(){
    std::cout << std::endl;
    std::cout << "* Deleting pointers" << std::endl;
    for(auto ptr: _drawing_ptrs){
      delete ptr;
    }
    _drawing_ptrs.clear();
  }

  void draw_u_ptrs(){
    _drawing_u_ptrs.clear();
    _drawing_u_ptrs.emplace_back(std::unique_ptr<Point>{new Point{10, 15}});
    _drawing_u_ptrs.emplace_back(getPointPtr(35, 22));
    _drawing_u_ptrs.emplace_back(getLinePtr(55, 122, 234, 556));
    _drawing_u_ptrs.emplace_back(getRectanglePtr(3, 194, 34, 200));
  }

private:

  // Collection of drawing elements
  std::vector<DrawingElement> _drawing;

  // Collection of pointers to drawing elements
  std::vector<DrawingElement*> _drawing_ptrs;

  // Collection of unique pointers to drawing elements
  std::vector<std::unique_ptr<DrawingElement>> _drawing_u_ptrs;

};

// --------------------------------------------------
// --------------------------------------------------
// --------------------------------------------------

int main(int argc, char* argv[])
{
  // Creating Drawing
  Drawing drawing;
  // drawing.draw();

  Point      point{11, 12};
  Line       line{22, 23, 124, 125};
  Rectangle  rect{33, 34, 234, 75};

  point.render();
  line.render();
  rect.render();

  // This doesn't work.
  // drawing.add_element(point);

  // The solution is to pass pointers
  drawing.add_element_ptr(&point);
  drawing.add_element_ptr(&line);
  drawing.add_element_ptr(&rect);
  drawing.render_ptrs();

  // Adding elements from a different scope
  drawing.draw_ptrs();
  // Unless elements are inserted with `new` operator,
  // trying to render will give runtime error:
  //    "pure virtual method called"
  drawing.render_ptrs();

  // To get around this, the pointers must be unique pointers
  drawing.draw_u_ptrs();
  drawing.render_u_ptrs();
}

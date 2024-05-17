#include "SVGElements.hpp"

namespace svg
{
    SVGElement::SVGElement() {}
    SVGElement::~SVGElement() {}
 
   
    // Ellipse (initial code provided)
    Ellipse::Ellipse(const Color &fill,
                     const Point &center,
                     const Point &radius)
        : fill(fill), center(center), radius(radius) {}    
                                            
    
    void Ellipse::draw(PNGImage &img) const { img.draw_ellipse(center, radius, fill); }

    void Ellipse::translate(int tx, int ty) {     
        center = center.translate({tx, ty});
    }

    void Ellipse::rotate(int v) {        
        center = center.rotate(center, v);
    }             

    void Ellipse::scale(int v) {           
        center = center.scale(center, v);
    }

    
    //Circle (Inherits transformations from Ellipse)
    Circle::Circle(const Color &fill, const Point &center, int radius): Ellipse(fill, center, {radius, radius}) {} 
    void Circle::draw(PNGImage &img) const {                                                                        
        Ellipse::draw(img); 
    }                                                   


    //Polyline
    Polyline::Polyline(const Color &stroke, const std::vector<Point> &points): stroke(stroke), points(points) {}
    void Polyline::draw(PNGImage &img) const {
    for (size_t i = 0; i < points.size() - 1; ++i) {                  
        img.draw_line(points[i], points[i + 1], stroke);               
    }
    }
    void Polyline::translate(int tx, int ty) {
    for (Point& point : points) {
        point = point.translate({tx, ty});
    }
    }

    void Polyline::rotate(int v) {
    Point transform_origin;
    for (Point& point : points) {                             
        point = point.rotate(transform_origin={0,0}, v);     
    }
    }

    void Polyline::scale(int v) {
    Point transform_origin;
    for (Point& point : points) {
        point = point.scale(transform_origin={0,0}, v);
    }
    }

    
    //Line (Inherits transformations from Polyline)
    Line::Line(Color stroke, Point p1, Point p2) : Polyline(stroke, {p1,p2}) {}
    void Line::draw(PNGImage &img) const {
        Polyline::draw(img);
    }

     //Polygon
    Polygon::Polygon(const Color &fill, const std::vector<Point> &points) : fill(fill), points(points) {}
    void Polygon::draw(PNGImage &img) const {
        img.draw_polygon(points, fill);
    }
    void Polygon::translate(int tx, int ty) {
    for (Point& point : points) {
        point = point.translate({tx, ty});
    }
    }

    void Polygon::rotate(int v) {
    Point transform_origin;
    for (Point& point : points) {                                   
        point = point.rotate(transform_origin={0,0}, v);          
    }
    }

    void Polygon::scale(int v) {
    Point transform_origin;
    for (Point& point : points) {
        point = point.scale(transform_origin={0,0}, v);
    }
    }


    //Rectangle (Inherits transformations from Polygon)
    Rect::Rect(const Color &fill, int x, int y, int width, int height) : Polygon(fill,  {{x, y}, {x + width, y}, {x + width, y + height}, {x, y + height}}) {}
    void Rect::draw(PNGImage &img) const {
    Polygon::draw(img);
    }

    Group::Group(const std::vector<SVGElement*>& elementos) : elementos(elementos) {}

    Group::~Group() {
        for (auto element : elementos) {
            delete element; // Free memory allocated for elements
        }
    }

    void Group::draw(PNGImage &img) const {
        for (auto element : elementos) {
            element->draw(img); // Draw each element in the group
        }
    }
}
    

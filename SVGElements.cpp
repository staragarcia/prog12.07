#include "SVGElements.hpp"

namespace svg
{
    // These must be defined!
    SVGElement::SVGElement() {}
    SVGElement::~SVGElement() {}

    // Ellipse (initial code provided)
    Ellipse::Ellipse(const Color &fill,
                     const Point &center,
                     const Point &radius)
        : fill(fill), center(center), radius(radius) {}
    
    void Ellipse::draw(PNGImage &img) const
    {
        img.draw_ellipse(center, radius, fill);
    }
    
    //Circle !!! 
    Circle::Circle(const Color &fill, const Point &center, int radius): Ellipse(fill, center, {radius, radius}) {}  // O raio na elipse é um ponto (2 raios),
    void Circle::draw(PNGImage &img) const {                                                                        //que aqui são iguais
        Ellipse::draw(img); 
    }                                                   


    //Polyline !!!
    Polyline::Polyline(const Color &stroke, const std::vector<Point> &points): stroke(stroke), points(points) {}
    void Polyline::draw(PNGImage &img) const {
    for (size_t i = 0; i < points.size() - 1; ++i) {                    //como não existe uma função draw_polyline em PNGimage, mas sim uma função draw_line,
        img.draw_line(points[i], points[i + 1], stroke);                // fazemos um ciclo que percorre os pontos e desenha linha a linha. 
    }
    }

    

    //Line !!!
    Line::Line(Color stroke, Point p1, Point p2) : Polyline(stroke, {p1,p2}) {}
    void Line::draw(PNGImage &img) const {
        Polyline::draw(img);
    }

}
    

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
                                            
    
    void Ellipse::draw(PNGImage &img) const { img.draw_ellipse(center, radius, fill); }

    //vamos implementar as funções de transform definidas em SVGElements.hpp (para a Ellipse)
    void Ellipse::translate(int tx, int ty) {        // translate já está definido em Point.cpp, na ellipse é só aplicar ao centro
        center = center.translate({tx, ty});
    }

    void Ellipse::rotate(int v) {                   // aplicamos rotate com v graus
        center = center.rotate(center, v);
    }             

    void Ellipse::scale(int v) {                    // aplicamos scale com fator v
        center = center.scale(center, v);
    }



    
    //Circle !!! 
    // as funções de transform são herdadas da ellipse, não temos de as declarar para o círculo yippee
    Circle::Circle(const Color &fill, const Point &center, int radius): Ellipse(fill, center, {radius, radius}) {}  // O raio na elipse é um ponto (2 raios),
    void Circle::draw(PNGImage &img) const {                                                                        // que aqui são iguais
        Ellipse::draw(img); 
    }                                                   


    //Polyline !!!
    Polyline::Polyline(const Color &stroke, const std::vector<Point> &points): stroke(stroke), points(points) {}
    void Polyline::draw(PNGImage &img) const {
    for (size_t i = 0; i < points.size() - 1; ++i) {                    //como não existe uma função draw_polyline em PNGimage, mas sim uma função draw_line,
        img.draw_line(points[i], points[i + 1], stroke);                // fazemos um ciclo que percorre os pontos e desenha linha a linha. 
    }
    }
    void Polyline::translate(int tx, int ty) {
    for (Point& point : points) {
        point = point.translate({tx, ty});
    }
    }

    void Polyline::rotate(int v) {
    Point transform_origin;
    for (Point& point : points) {                                   // eu ns se isto ta certo mas se o transform_origin n existir (n o extrairmos do codigo)
        point = point.rotate(transform_origin={0,0}, v);            // ele vai ser {0,0} como é pedido no projeto;
    }
    }

    void Polyline::scale(int v) {
    Point transform_origin;
    for (Point& point : points) {
        point = point.scale(transform_origin={0,0}, v);
    }
    }

    
    //Line !!!
    // a Line também herda da polyline :)
    Line::Line(Color stroke, Point p1, Point p2) : Polyline(stroke, {p1,p2}) {}
    void Line::draw(PNGImage &img) const {
        Polyline::draw(img);
    }

     //Polygon !!!
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
    for (Point& point : points) {                                   // eu ns se isto ta certo mas se o transform_origin n existir (n o extrairmos do codigo)
        point = point.rotate(transform_origin={0,0}, v);            // ele vai ser {0,0} como é pedido no projeto;
    }
    }

    void Polygon::scale(int v) {
    Point transform_origin;
    for (Point& point : points) {
        point = point.scale(transform_origin={0,0}, v);
    }
    }




    //Rectangle !!!
    Rect::Rect(const Color &fill, int x, int y, int width, int height) : Polygon(fill,  {{x, y}, {x + width, y}, {x + width, y + height}, {x, y + height}}) {}
    void Rect::draw(PNGImage &img) const {
    Polygon::draw(img);
    }


}
    

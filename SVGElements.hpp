//! @file shape.hpp
#ifndef __svg_SVGElements_hpp__
#define __svg_SVGElements_hpp__

#include "Color.hpp"
#include "Point.hpp"
#include "PNGImage.hpp"
using namespace std;


namespace svg
{
    class SVGElement
    {

    public:
        SVGElement();
        virtual ~SVGElement();
        virtual void draw(PNGImage &img) const = 0;
        virtual void translate(int x, int y) = 0;
        virtual void rotate(int v) = 0;
        virtual void scale(int v) = 0;
    };

    // Declaration of namespace functions
    // readSVG -> implement it in readSVG.cpp
    // convert -> already given (DO NOT CHANGE) in convert.cpp

    void readSVG(const std::string &svg_file,
                 Point &dimensions,
                 std::vector<SVGElement *> &svg_elements);
    void convert(const std::string &svg_file,
                 const std::string &png_file);


 
    
    // ELLIPSE
    class Ellipse : public SVGElement
    {
    public:
        //! Ellipse constructor
        //! @param fill Fill color;
        //! @param center Center of the Ellipse;
        //! @param radius Point that holds the Ellipse's 2 radiuses;
        Ellipse(const Color &fill, const Point &center, const Point &radius);

        //! Draws the ellipse
        //! @param img PNGImage object where the element will be drawn.
        void draw(PNGImage &img) const override;

        //! Translates the SVG element.
        //! @param tx Translation along the x-axis.
        //! @param ty Translation along the y-axis.
        void translate(int tx, int ty) override;

        //! Rotates the SVG element.
        //! @param v Rotation angle in degrees.
        void rotate(int v) override;

        //! Scales the SVG element.
        //! @param v Scaling factor.
        void scale(int v) override;

    private:
        Color fill;     //! Fill color.
        Point center;   //! Center of the Ellipse.
        Point radius;   //! Point that holds the Ellipse's 2 radiuses.
    };


    //CIRCLE (SUBCLASSE DE ELLIPSE)
    class Circle : public Ellipse 
    {
    public:
        //! Circle constructor.
        //! @param fill Fill color.
        //! @param center Center of the Circle.
        //! @param radius Radius of the Circle.
        Circle(const Color &fill, const Point &center, const int radius);

        //! Draws the circle.
        //! @param img PNGImage object where the circle will be drawn.
        void draw(PNGImage &img) const override;
    };

    //POLYLINE
    class Polyline : public SVGElement
    {
    public:
        //! Polyline constructor.
        //! @param stroke Stroke color.
        //! @param points Vector of points defining the Polyline.
        Polyline(const Color &stroke, const std::vector<Point> &points);

        //! Draws the polyline.
        //! @param img PNGImage object where the polyline will be drawn.
        void draw(PNGImage &img) const override;

        //! Translates the Polyline.
        //! @param tx Translation along the x-axis.
        //! @param ty Translation along the y-axis.
        void translate(int tx, int ty) override;

        //! Rotates the Polyline.
        //! @param v Rotation angle in degrees. 
        void rotate(int v) override;

        //! Scales the Polyline.
        //! @param v Scaling factor.
        void scale(int v) override;

    private:
        Color stroke;                   //! Stroke color.
        vector<Point> points;           //! Vector of points defining the Polyline.
    };

    //LINE (SUBCLASS OF POLYLINE)
    class Line : public Polyline 
    {
    public:
        
        //! Line constructor.
        //! @param stroke Stroke color.
        //! @param p1 Line starting point.
        //! @param p2 Line ending point.
        Line(Color stroke, Point p1, Point p2);

        //! Draws the line.
        //! @param img PNGImage object where the line will be drawn.
        void draw(PNGImage &img) const override;
    };

    //POLYGON
    class Polygon : public SVGElement 
    {
    public:

        //! Polygon constructor.
        //! @param fill Fill color.
        //! @param points Vector of points defining the Polygon.
        Polygon(const Color &fill, const std::vector<Point> &points);

        //! Draws the polygon.
        //! @param img PNGImage object where the polygon will be drawn.
        void draw(PNGImage &img) const override;

        //! Translates the Polygon.
        //! @param tx Translation along the x-axis.
        //! @param ty Translation along the y-axis.
        void translate(int tx, int ty) override;

        //! Rotates the Polygon.
        //! @param v Rotation angle in degrees.
        void rotate(int v) override;

        //! Scales the Polygon.
        //! @param v Scaling factor.
        void scale(int v) override;

        private:
            Color fill;             //! Fill color.
            vector<Point> points;   //! Vector of points defining the Polygon.
    };


    // RECTANGLE (SUBCLASSE DE POLYGON)
    class Rect : public Polygon
    {
        public:
            //! Rectangle constructor;
            //! @param fill Fill color.
            //! @param x x coordinate of the top-left point.
            //! @param y y coordinate of the top-left point.
            //! @param width width of the rectangle.
            //! @param height height of the rectangle.
            Rect(const Color &fill, int x, int y, int width, int height);

            //! Draws the rectangle.
            //! @param img PNGImage object where the rectangle will be drawn.
            void draw(PNGImage &img) const override;
    };
    
    
    class Group : public SVGElement 
    {
    public:
        //! Constructs a Group object with a vector of SVG elements.
        //! @param elementos Vector of pointers to SVGElement objects that form the group.
        Group(const vector<SVGElement*>& elementos);

        //! Destructor for the Group object.
        ~Group();

        //! Draws the group elements
        //! @param img PNGImage object where the elements will be drawn.
        void draw(PNGImage &img) const override; 

    private:
        vector<SVGElement*> elementos; //! Vector of pointers to SVGElement objects that form the group.
    };

}
#endif

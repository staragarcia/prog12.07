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

    class Group : public SVGElement
    {
    public:
        Group();
        ~Group();
        void draw(PNGImage &img) const override;
        void translate(int x, int y) override;
        void rotate(int v) override;
        void scale(int v) override;
        void addElement(SVGElement *element);

    private:
        std::vector<SVGElement *> elements;
    };

    class Ellipse : public SVGElement
    {
    public:
        Ellipse(const Color &fill, const Point &center, const Point &radius);
        void draw(PNGImage &img) const override;
        void translate(int tx, int ty) override;
        void rotate(int v) override;
        void scale(int v) override;

    private:
        Color fill;
        Point center;
        Point radius;
    };


    //CIRCLE (SUBCLASSE DE ELLIPSE)
    class Circle : public Ellipse 
    {
    public:
        Circle(const Color &fill, const Point &center, const int radius);
        void draw(PNGImage &img) const override;
    };

    //POLYLINE
    class Polyline : public SVGElement
    {
    public:
        Polyline(const Color &stroke, const std::vector<Point> &points);
        void draw(PNGImage &img) const override;
        void translate(int tx, int ty) override;
        void rotate(int v) override;
        void scale(int v) override;

    private:
        Color stroke;
        vector<Point> points;
    };

    //LINE
    class Line : public Polyline 
    {
    public:
        Line(Color stroke, Point p1, Point p2);
        void draw(PNGImage &img) const override;
    };

    //POLYGON
    class Polygon : public SVGElement 
    {
        public:
            Polygon(const Color &fill, const std::vector<Point> &points);
            void draw(PNGImage &img) const override;
            void translate(int tx, int ty) override;
            void rotate(int v) override;
            void scale(int v) override;

        private:
            Color fill;
            vector<Point> points;
    };

    // RECTANGLE
    class Rect : public Polygon
    {
        public:
            Rect(const Color &fill, int x, int y, int width, int height);
            void draw(PNGImage &img) const override;
    };

}
#endif


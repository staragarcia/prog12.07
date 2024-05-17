#include <iostream>
#include "SVGElements.hpp"
#include "external/tinyxml2/tinyxml2.h"
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace std;
using namespace svg;
using namespace tinyxml2;


namespace svg

{   // Function that turns a point string into a vector of points;                                          
    vector<Point> str_to_vec(const string& str) {               
    vector<Point> points;
    istringstream iss(str); 

    char virg;
    Point point;
    while (iss >> point.x >> virg >> point.y) { 
        points.push_back(point);
        if (iss.peek() == ',') {                        
            iss.ignore(); }                                        
    }
    return points;
    }
    

    //Extracting the transformation:

    struct transformacao {
    string tipo;                // Type of transformation;  
    vector<int> argumentos;     // Vector of arguments     
    };

    //Function that extracts the transformation from a string:
    transformacao extractTransform(string s) {
        transformacao result;
        string type;

        istringstream iss(s);

        getline(iss, result.tipo, '(');                                 //The type is extracted until we find an '(';

    int argument;                                                        
    while (iss >> argument) {                                           //Adding the arguments to the argument vector;
        result.argumentos.push_back(argument);
        if (iss.peek() == ',') {                                        //Accepting a ',' instead of a ' ' between arguments;
            iss.ignore(); 
        }
        if (iss.peek() == ')') {                                  
            iss.ignore();
            break;
        }
    }
    return result;
}

   
    void trata_grupo(XMLElement* e, vector<SVGElement *>& svg_elements);                            //Declaration of group function;

    void readSVG(const string& svg_file, Point& dimensions, vector<SVGElement *>& svg_elements)     //Function for reading SVG files:
    {
        XMLDocument doc;
        XMLError r = doc.LoadFile(svg_file.c_str()); 
        if (r != XML_SUCCESS)
        {
            throw runtime_error("Unable to load " + svg_file);
        }
        XMLElement *xml_elem = doc.RootElement();

        dimensions.x = xml_elem->IntAttribute("width");
        dimensions.y = xml_elem->IntAttribute("height");
        
        // TODO complete code -->

        XMLElement *element = xml_elem->FirstChildElement();    // Get the first child element of xml_elem, assigning it to the pointer 'element'.
                                                                // If there is no first child, FirstChildElement() returns nullptr.
        while (element != nullptr) {

             
        if (strcmp(element->Name(), "ellipse") == 0) {          // Check if the current element is an ellipse;
            int cx = element->IntAttribute("cx");               // Extract the "cx", "cy", "rx", and "ry" attributes;
            int cy = element->IntAttribute("cy");
            int rx = element->IntAttribute("rx"); 
            int ry = element->IntAttribute("ry");
            string fill_color = element->Attribute("fill");     // Extract the "fill" attribute;

            Point center;                                       // Create a point for the center of the ellipse;
            center.x = cx;
            center.y = cy;
            
            Point radius;                                       // Create a point object for the radiuses of the ellipse (a point because there's 2);
            radius.x = rx;
            radius.y = ry;

            
            Color fill = parse_color(fill_color);               // Turn the extracted color into a Color object using parse_color();
            
            // Convert the transform attribute to a string and extract the transformation data:
            const char *trueform = element->Attribute("transform");             
            if (trueform != nullptr) {                                       
                string transString = trueform;                                  
                transformacao transf = extractTransform(transString);         

                // Apply the transformations translate, rotate and scale:
                if ( transf.tipo == "translate" ) {                                         
                center = center.translate({transf.argumentos[0], transf.argumentos[1]});                //Apply translation to the center of the Ellipse
                }

                else if (transf.tipo == "rotate") {
                    Point origin = {0, 0};
                    const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                    if (trans_origin) {
                        istringstream iss(trans_origin);
                        iss >> origin.x >> origin.y;                                                    //Build origin point;
                        }
                    center = center.rotate(origin, transf.argumentos[0]);
                } 

                else if ( transf.tipo == "scale" ) {
                    Point origin = {0, 0};
                    const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                    if (trans_origin) {
                        istringstream iss(trans_origin);
                        iss >> origin.x >> origin.y;                                                    //Build origin point;
                        }
                    int scale_factor = transf.argumentos[0];
                    if (scale_factor >= 1) {
                        radius.x *= scale_factor;
                        radius.y *= scale_factor;
                        center = center.scale(origin, scale_factor);
                    }
                                        
                }

            }
            // Create a new Ellipse object and add it to the svg_elements vector. 
            svg_elements.push_back(new Ellipse(fill, center, radius));
        }

        //Do the same for the remaining elements: circle, polyline, line, polygon and rectangle;
        else if (strcmp(element->Name(), "circle") == 0) {
            int cx = element->IntAttribute("cx");
            int cy = element->IntAttribute("cy");
            int r = element->IntAttribute("r");
            string fill_color = element->Attribute("fill");

            Point center;
            center.x = cx;
            center.y = cy;

            Color fill = parse_color(fill_color);

            const char* trueform = element->Attribute("transform");     
            if (trueform) {                                                 
                string transString = trueform;                                  
                transformacao transf = extractTransform(transString);           

                if ( transf.tipo == "translate" ) {
                center = center.translate({transf.argumentos[0], transf.argumentos[1]});
                }

                else if (transf.tipo == "rotate") {
                Point origin = {0, 0};
                    const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;                      
                    if (trans_origin) {
                        istringstream iss(trans_origin);
                        iss >> origin.x >> origin.y; }                                                  //Build origin point;
                center = center.rotate(origin, transf.argumentos[0]);
                }          

                else if ( transf.tipo == "scale" ) {
                    Point origin = {0, 0};
                    const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                    if (trans_origin) {
                        istringstream iss(trans_origin);
                        iss >> origin.x >> origin.y; }
                    
                    int scale_factor = transf.argumentos[0];
                    if (scale_factor >= 1) {
                        r *= scale_factor;
                    center = center.scale(origin, scale_factor);                                        
                    }
                                
                }

            }

            svg_elements.push_back(new Circle(fill, center, r));
        }


        else if (strcmp(element->Name(), "polyline") == 0) {    

            string line_color = element->Attribute("stroke");
            Color stroke = parse_color(line_color);
            string line_points = element->Attribute("points");              // Since the points are given to us as a string, we must turn them into a vector,
            vector<Point> points = str_to_vec(line_points);                 // using str_to_vec previously defined;

            const char *trueform = element->Attribute("transform"); 
            if (trueform != nullptr) {                               
                string transString = trueform;                              
                transformacao transf = extractTransform(transString);       

                if ( transf.tipo == "translate" ) {
                    for (Point& point : points) {
                    point = point.translate({transf.argumentos[0], transf.argumentos[1]});
                    }
                }

                else if (transf.tipo == "rotate") {
                    Point origin = {0, 0};
                    const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                    if (trans_origin) {
                        istringstream iss(trans_origin);
                        iss >> origin.x >> origin.y; 
                    }
                    for (Point& point : points) {
                    point = point.rotate(origin, transf.argumentos[0]);
                    }
                } 

                else if (transf.tipo == "scale") {
                    Point origin = {0, 0};
                    const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                    if (trans_origin) {
                        istringstream iss(trans_origin);
                        iss >> origin.x >> origin.y; 
                    }
                    for (Point& point : points) {
                        point = point.scale(origin, transf.argumentos[0]);
                    }
                }
            }
            svg_elements.push_back(new Polyline(stroke, points));
        }

        else if (strcmp(element->Name(), "line") == 0) {
                                      
            string line_color = element->Attribute("stroke");
            Color stroke = parse_color(line_color);

            //Pontos:
            int x1 = element->IntAttribute("x1");
            int y1 = element->IntAttribute("y1");
            int x2 = element->IntAttribute("x2");
            int y2 = element->IntAttribute("y2");

            Point p1 = {x1, y1};
            Point p2 = {x2, y2};

            const char *trueform = element->Attribute("transform");        
        if (trueform != nullptr) {                                   
            string transString = trueform;                            
            transformacao transf = extractTransform(transString);   

            if ( transf.tipo == "translate" ) {
                p1 = p1.translate({transf.argumentos[0], transf.argumentos[1]});
                p2 = p2.translate({transf.argumentos[0], transf.argumentos[1]});
                
            }

            else if (transf.tipo == "rotate") {
                Point origin = {0, 0};
                const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                if (trans_origin) {
                    istringstream iss(trans_origin);
                    iss >> origin.x >> origin.y; 
                }
                p1 = p1.rotate(origin, transf.argumentos[0]);
                p2 = p2.rotate(origin, transf.argumentos[0]);
            }

            if (transf.tipo == "scale") {
            int scale_factor = transf.argumentos[0];
            if (scale_factor >= 1) {
                Point origin = {0, 0};
                const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                if (trans_origin) {
                    istringstream iss(trans_origin);
                    iss >> origin.x >> origin.y; 
                }
                p1 = p1.scale(origin, scale_factor);
                p2 = p2.scale(origin, scale_factor);
            }
        }
        }
            
            svg_elements.push_back(new Line(stroke, p1, p2));

        }

        else if (strcmp(element->Name(), "polygon") == 0) {   

            string fill_color = element->Attribute("fill");    
            string line_points = element->Attribute("points");

            Color fill = parse_color(fill_color);   
            vector<Point> points = str_to_vec(line_points);

            const char *trueform = element->Attribute("transform");  
            if (trueform != nullptr) {                                  
                string transString = trueform;                           
                transformacao transf = extractTransform(transString);   

            if ( transf.tipo == "translate" ) {
                for (Point& point : points) {
                point = point.translate({transf.argumentos[0], transf.argumentos[1]});
                }
            }

            else if (transf.tipo == "rotate") {
                Point origin = {0, 0};
                const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                if (trans_origin) {
                    istringstream iss(trans_origin);
                    iss >> origin.x >> origin.y; 
                }
                for (Point& point : points) {
                point = point.rotate(origin, transf.argumentos[0]);
                }
                
            }

            else if (transf.tipo == "scale") {
            int scale_factor = transf.argumentos[0];
                Point origin = {0, 0};
                const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                if (trans_origin) {
                    istringstream iss(trans_origin);
                    iss >> origin.x >> origin.y; 
                }
                for (Point& point : points) {
                    point = point.scale(origin, scale_factor);
                }

            }

            }

            svg_elements.push_back(new Polygon(fill, points));

        }

        else if (strcmp(element->Name(), "rect") == 0) {   
            bool rodou = false;
          
            int x = element->IntAttribute("x");
            int y = element->IntAttribute("y");
            int rwidth = element->IntAttribute("width");
            int rheight = element->IntAttribute("height");
            string fill_color = element->Attribute("fill");
        
            Color fill = parse_color(fill_color); 

            const char *trueform = element->Attribute("transform");           
            if (trueform != nullptr) {                                        
                string transString = trueform;                                
                transformacao transf = extractTransform(transString);     

            if ( transf.tipo == "translate" ) {
                x += transf.argumentos[0];                                      // Translate manually the x coordinate
                y += transf.argumentos[1];                                      // Translate manually the y coordinate
                }

            else if (transf.tipo == "rotate") {                                 // Rotated rectangles will be added as polygons;
                rodou = true;                                                   // Flags a rotation;
                Point origin = {0, 0};
                const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                if (trans_origin) {
                    istringstream iss(trans_origin);
                    iss >> origin.x >> origin.y; 
                }
                Point topleft = {x, y};                                                             // Creating the 4 points of the rectangle;
                Point topright = {x + rwidth-1, y};
                Point bottomleft = {x, y + rheight-1};
                Point bottomright = {x + rwidth-1, y + rheight-1};

                vector<Point> points;

                points.push_back(topleft.rotate(origin, transf.argumentos[0]));                     // Adding rotated rectangle points to a vector;
                points.push_back(topright = topright.rotate(origin, transf.argumentos[0]));
                points.push_back(bottomright.rotate(origin, transf.argumentos[0]));
                points.push_back(bottomleft = bottomleft.rotate(origin, transf.argumentos[0]));

                svg_elements.push_back(new Polygon(fill,points));                                   // Use the obtained vector to create a Polygon.

    
            }

            else if (transf.tipo == "scale") {
            int scale_factor = transf.argumentos[0];
                Point origin = {0, 0};
                const char* trans_origin = element->Attribute("transform-origin");                  //Extract transform-origin;
                if (trans_origin) {
                    istringstream iss(trans_origin);
                    iss >> origin.x >> origin.y; 
                }
                rwidth =  (rwidth)*scale_factor - scale_factor +1 ;
                rheight = (rheight)*scale_factor - scale_factor +1 ;
                x = origin.x + scale_factor * (x - origin.x);
                y = origin.y + scale_factor * (y - origin.y);

            } 
        }

            if (!rodou) svg_elements.push_back(new Rect(fill, x, y, rwidth-1, rheight-1));          // We don't create a rectangle for rotion, as a
                                                                                                    // polygon has already been created;
        }

        if (strcmp(element->Name(), "g") == 0) {                                                    // Identifying a group element;
                trata_grupo(element,svg_elements);                                                  // Jump to a function that will handle the groups;
        }

        element = element->NextSiblingElement();


        }                
    }

     void trata_grupo(XMLElement* e, vector<SVGElement *>& svg_elements) {                          // Function that handles group elements;
        const char *trueform = e->Attribute("transform");                                           // Extract group transformation;
        transformacao transf;
            if (trueform != nullptr) {                                                              
                string transString = trueform;                          
                transf = extractTransform(transString);
                }
        XMLElement *element_neto = e->FirstChildElement();                                          // Get the first child element of e;
        while (element_neto != nullptr) {
            
            if (strcmp(element_neto->Name(), "g") == 0) {                                           // Recursion for groups inside other groups;
                  trata_grupo(element_neto,svg_elements);
            }
            else {
                    // Treating the different kinds of elements inside groups;
                    if (strcmp(element_neto->Name(), "circle") == 0) {
                        int cx = element_neto->IntAttribute("cx");
                        int cy = element_neto->IntAttribute("cy");
                        int r = element_neto->IntAttribute("r");
                        string fill_color = element_neto->Attribute("fill");

                        Point center;
                        center.x = cx;
                        center.y = cy;

                        Color fill = parse_color(fill_color);
                             
                            if ( transf.tipo == "translate" ) {
                            center = center.translate({transf.argumentos[0], transf.argumentos[1]});
                            }

                            else if (transf.tipo == "rotate") {
                            Point origin = {0, 0};
                                const char* trans_origin = element_neto->Attribute("transform-origin");     //Extract transform-origin;
                                if (trans_origin) {
                                    istringstream iss(trans_origin);
                                    iss >> origin.x >> origin.y; }
                            center = center.rotate(origin, transf.argumentos[0]);
                            }          

                            else if ( transf.tipo == "scale" ) {
                                Point origin = {0, 0};
                                const char* trans_origin = element_neto->Attribute("transform-origin");     //Extract transform-origin;
                                if (trans_origin) {
                                    istringstream iss(trans_origin);
                                    iss >> origin.x >> origin.y; }
                                
                                int scale_factor = transf.argumentos[0];
                                if (scale_factor >= 1) {
                                    r *= scale_factor;
                                center = center.scale(origin, scale_factor);
                                }           
                            }
                        
                        svg_elements.push_back(new Circle(fill, center, r));
                    }

                    if (strcmp(element_neto->Name(), "polygon") == 0) { 
                        string fill_color = element_neto->Attribute("fill");    
                        string line_points = element_neto->Attribute("points");

                        Color fill = parse_color(fill_color);   
                        vector<Point> points = str_to_vec(line_points);

        

                        if ( transf.tipo == "translate" ) {
                            for (Point& point : points) {
                            point = point.translate({transf.argumentos[0], transf.argumentos[1]});
                            }
                        }

                        else if (transf.tipo == "rotate") {
                            Point origin = {0, 0};
                            const char* trans_origin = element_neto->Attribute("transform-origin");
                            if (trans_origin) {
                                istringstream iss(trans_origin);
                                iss >> origin.x >> origin.y; 
                            }
                            for (Point& point : points) {
                            point = point.rotate(origin, transf.argumentos[0]);
                            }
                            
                        }

                        else if (transf.tipo == "scale") {
                        int scale_factor = transf.argumentos[0];
                            Point origin = {0, 0};
                            const char* trans_origin = element_neto->Attribute("transform-origin");
                            if (trans_origin) {
                                istringstream iss(trans_origin);
                                iss >> origin.x >> origin.y; 
                            }
                            for (Point& point : points) {
                                point = point.scale(origin, scale_factor);
                            }
                        }
                        svg_elements.push_back(new Polygon(fill, points));
                    }
            }
            
            element_neto = element_neto->NextSiblingElement(); 
        } 
    }
}

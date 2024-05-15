#include <iostream>
#include "SVGElements.hpp"
#include "external/tinyxml2/tinyxml2.h"
#include <sstream>

using namespace std;
using namespace svg;
using namespace tinyxml2;

namespace svg

{
                                                                        
    vector<Point> str_to_vec(const std::string& str) {                      // função que transforma a string num vetor de pontos
    vector<Point> points;
    istringstream iss(str);                                                 // str é o input
      
    char virg;
    Point point;
    while (iss >> point.x >> virg >> point.y) {                             // point.x <- int , virgula, point.y <- int;
        points.push_back(point);
        if (iss.peek() == ',') {                                            // esta função (peek) vê se a seguir ao ponto lido vem uma vírgula 
            iss.ignore(); }                                                 // e se vier ignora :)
    }
    return points;
    }
    

    //EXTRAIR A TRANSFORMAÇÃO
    struct transformação {
    string tipo;                                                        // O tipo
    vector<int> argumentos;                                             // Há sempre um ou mais argumentos inteiros
    };

    transformação extractTransform(string s) {
        transformação result;
        string type;

        istringstream iss(s);

        getline(iss, result.tipo, '(');                                 // O tipo é xtraído até encontrar um '(' e diretamente guardado em result.tipo

    int argument;                                                        
    while (iss >> argument) {                                           // Como estão separados por um espaço podemos pôr os argumentos logo no vetor do result
        result.argumentos.push_back(argument);

        if (iss.peek() == ',') {                                        // Por causa do reste translate_rect, temos de aceitar (ignorar a vírgula no meio)
            iss.ignore(); 
        }
        
        if (iss.peek() == ')') {                                        // Se o próximo character for uma ')' acaba a leitura
            iss.ignore();
            break;
        }
    }
    return result;
}



    void readSVG(const string& svg_file, Point& dimensions, vector<SVGElement *>& svg_elements)
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
        XMLElement *element = xml_elem->FirstChildElement();    // element é ponteiro para o "FirstChildElement" de xml_elem
                                                                // se não exsitir uma FirstChild, aquela função retorna o nullptr
        while (element != nullptr) {
    
        if (strcmp(element->Name(), "ellipse") == 0) {          // comparar o nome do element com "ellipse". se for igual strcmp=0, temos um ellipse element)
            int cx = element->IntAttribute("cx");
            int cy = element->IntAttribute("cy");               // fazer o IntAttribute do codigo SVG (pegar no "cx" do codigo por exemplo, relativamente ao
            int rx = element->IntAttribute("rx");               // centro, e guardar esse valor em cx (um valor int), neste caso a coordenada x do centro)
            int ry = element->IntAttribute("ry");
            string fill_color = element->Attribute("fill");     // para perceber isto dá jeito ir a input -> ellipse_1.svg dá pra ver lá o código SVG :))

            //como Point é um struct e não uma classe com constructors, lá vamos nós criar o centro e o raio:
            Point center;
            center.x = cx;
            center.y = cy;

            Point radius;
            radius.x = rx;
            radius.y = ry;

            //a color também é um struct mas pode vir como hex code ou string, para isso temos a função parse_color definida em Color.cpp:
            Color fill = parse_color(fill_color);
            
        //MAGIA NEGRA: (em processo) FUNCIONA CARALHO LETS GOOOOOO
        const char *trueform = element->Attribute("transform");             // Encontrar o texto do transform
        if (trueform != nullptr) {                                          // se não estiver vazio:
            string transString = trueform;                                  // guardar no transString
            transformação coiso = extractTransform(transString);            // aplicar extractTransform a transString e guardar em coiso;

            if ( coiso.tipo == "translate" ) {
            center = center.translate({coiso.argumentos[0], coiso.argumentos[1]});
            }


            else if ( coiso.tipo == "rotate" ) {
                string trans_origin = element->Attribute("transform-origin");
                Point origin;
                istringstream iss(trans_origin);
                char space = ' ';                                       
                iss >> origin.x >> space >> origin.y; 
                center.rotate(center, coiso.argumentos[0]);
            }             

            else if ( coiso.tipo == "scale" ) {
                string trans_origin = element->Attribute("transform-origin");
                Point origin;
                istringstream iss(trans_origin);
                char space = ' ';
                iss >> origin.x >> space >> origin.y; 
                center.scale(center, coiso.argumentos[0]);
                                
                }

            }

            svg_elements.push_back(new Ellipse(fill, center, radius));
        }

        else if (strcmp(element->Name(), "circle") == 0)
        {
            int cx = element->IntAttribute("cx");
            int cy = element->IntAttribute("cy");
            int r = element->IntAttribute("r");
            string fill_color = element->Attribute("fill");

            Point center;
            center.x = cx;
            center.y = cy;

            Color fill = parse_color(fill_color);


            const char* trueform = element->Attribute("transform");             // Encontrar o texto do transform
            if (trueform) {                                                     // se não estiver vazio:
                string transString = trueform;                                  // guardar no transString
                transformação coiso = extractTransform(transString);            // aplicar extractTransform a transString e guardar em coiso;

            if ( coiso.tipo == "translate" ) {
            center = center.translate({coiso.argumentos[0], coiso.argumentos[1]});
            }


            else if ( coiso.tipo == "rotate" ) {
                string trans_origin = element->Attribute("transform-origin");
                Point origin;
                istringstream iss(trans_origin);
                char space = ' ';                                           //Inês ajudou 👏
                iss >> origin.x >> space >> origin.y; 
                center.rotate(center, coiso.argumentos[0]);
            }             

            else if ( coiso.tipo == "scale" ) {
                string trans_origin = element->Attribute("transform-origin");
                Point origin;
                istringstream iss(trans_origin);
                char space = ' ';
                iss >> origin.x >> space >> origin.y; 
                center.scale(center, coiso.argumentos[0]);
                                
                }

            }

            svg_elements.push_back(new Circle(fill, center, r));
        }


        else if (strcmp(element->Name(), "polyline") == 0) {        // os parâmetros da polyline são a stroke (cor) e o vetor de pontos;
                
                                                                   
            string line_color = element->Attribute("stroke");
            Color stroke = parse_color(line_color);
            string line_points = element->Attribute("points");      //como nos dão os pontos numa string, temos de os separar e guardar num vetor 
            vector<Point> points = str_to_vec(line_points);         // Função definida na linha 13


        const char *trueform = element->Attribute("transform");             // Encontrar o texto do transform
        if (trueform != nullptr) {                                          // se não estiver vazio:
            string transString = trueform;                                  // guardar no transString
            transformação coiso = extractTransform(transString);            // aplicar extractTransform a transString e guardar em coiso;

            if ( coiso.tipo == "translate" ) {
                for (Point& point : points) {
                point = point.translate({coiso.argumentos[0], coiso.argumentos[1]});
                }
            }
        }

            
            svg_elements.push_back(new Polyline(stroke, points));

        }

        else if (strcmp(element->Name(), "line") == 0) {            // como no círculo e na ellipse basicamente
                                      
            string line_color = element->Attribute("stroke");
            Color stroke = parse_color(line_color);

            //Pontos:
            int x1 = element->IntAttribute("x1");
            int y1 = element->IntAttribute("y1");
            int x2 = element->IntAttribute("x2");
            int y2 = element->IntAttribute("y2");

            Point p1 = {x1, y1};
            Point p2 = {x2, y2};

            const char *trueform = element->Attribute("transform");         // Encontrar o texto do transform
        if (trueform != nullptr) {                                          // se não estiver vazio:
            string transString = trueform;                                  // guardar no transString
            transformação coiso = extractTransform(transString);            // aplicar extractTransform a transString e guardar em coiso;

            if ( coiso.tipo == "translate" ) {
                p1 = p1.translate({coiso.argumentos[0], coiso.argumentos[1]});
                p2 = p2.translate({coiso.argumentos[0], coiso.argumentos[1]});
                
            }
        }
            
            svg_elements.push_back(new Line(stroke, p1, p2));

        }

        else if (strcmp(element->Name(), "polygon") == 0) {   

            string fill_color = element->Attribute("fill");    
            string line_points = element->Attribute("points");

            Color fill = parse_color(fill_color);   
            vector<Point> points = str_to_vec(line_points);

            const char *trueform = element->Attribute("transform");             // Encontrar o texto do transform
            if (trueform != nullptr) {                                          // se não estiver vazio:
                string transString = trueform;                                  // guardar no transString
                transformação coiso = extractTransform(transString);            // aplicar extractTransform a transString e guardar em coiso;

            if ( coiso.tipo == "translate" ) {
                for (Point& point : points) {
                point = point.translate({coiso.argumentos[0], coiso.argumentos[1]});
                }
            }
            }

            svg_elements.push_back(new Polygon(fill, points));

        }

        else if (strcmp(element->Name(), "rect") == 0) {   
          
            int x = element->IntAttribute("x");
            int y = element->IntAttribute("y");
            int rwidth = element->IntAttribute("width");
            int rheight = element->IntAttribute("height");
            string fill_color = element->Attribute("fill");
        
            Color fill = parse_color(fill_color); 

            const char *trueform = element->Attribute("transform");             // Encontrar o texto do transform
            if (trueform != nullptr) {                                          // se não estiver vazio:
                string transString = trueform;                                  // guardar no transString
                transformação coiso = extractTransform(transString);            // aplicar extractTransform a transString e guardar em coiso;

            if ( coiso.tipo == "translate" ) {
                x += coiso.argumentos[0];;                                      // Translate manualmente a coordenada x
                y += coiso.argumentos[1];                                       // Translate manualmente a coordenada y
                }
            }

            svg_elements.push_back(new Rect(fill, x, y, rwidth, rheight));
        }


        element = element->NextSiblingElement(); // próximo elemento 


        }                
    }
}                  

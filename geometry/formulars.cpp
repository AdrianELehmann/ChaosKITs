// Komplexe Zahlen als Darstellung für Punkte. Wenn immer möglich
// complex<int> verwenden. Funktionen wie abs() geben dann int zurück.
typedef complex<double> pt;

// Winkel zwischen Punkt und x-Achse in [0, 2 * PI).
double angle = arg(a);

// Punkt rotiert um Winkel theta.
pt a_rotated = a * exp(pt(0, theta));

// Mittelpunkt des Dreiecks abc.
pt centroid = (a + b + c) / 3.0;

// Skalarprodukt.
double dot(pt a, pt b) { return real(conj(a) * b); }

// Kreuzprodukt, 0, falls kollinear.
double cross(pt a, pt b) { return imag(conj(a) * b); }

// Flächeninhalt eines Dreicks bei bekannten Eckpunkten.
double areaOfTriangle(pt a, pt b, pt c) {
	return abs(cross(b - a, c - a)) / 2.0;
}

// Flächeninhalt eines Dreiecks bei bekannten Seitenlängen.
double areaOfTriangle(double a, double b, double c) {
	double s = (a + b + c) / 2;
	return sqrt(s * (s-a) * (s-b) * (s-c));
}

// Sind die Dreiecke a1, b1, c1, and a2, b2, c2 ähnlich?
// Erste Zeile testet Ähnlichkeit mit gleicher Orientierung,
// zweite Zeile testet Ähnlichkeit mit unterschiedlicher Orientierung
bool similar (pt a1, pt b1, pt c1, pt a2, pt b2, pt c2) {
	return (
		(b2-a2) * (c1-a1) == (b1-a1) * (c2-a2) ||
		(b2-a2) * (conj(c1)-conj(a1)) == (conj(b1)-conj(a1)) * (c2-a2)
	);
}

// -1 => gegen den Uhrzeigersinn, 0 => kolliniear, 1 => im Uhrzeigersinn.
// Einschränken der Rückgabe auf [-1,1] ist sicherer gegen Overflows.
double orientation(pt a, pt b, pt c) {
	double orien = cross(b - a, c - a);
	if (abs(orien) < EPSILON) return 0; // Braucht großes EPSILON: ~1e-6
	return orien < 0 ? -1 : 1;
}

// Test auf Streckenschnitt zwischen a-b und c-d.
bool lineSegmentIntersection(pt a, pt b, pt c, pt d) {
	if (orientation(a, b, c) == 0 && orientation(a, b, d) == 0) {
		double dist = abs(a - b);
		return (abs(a - c) <= dist && abs(b - c) <= dist) ||
					 (abs(a - d) <= dist && abs(b - d) <= dist);
	}
	return orientation(a, b, c) * orientation(a, b, d) <= 0 &&
				 orientation(c, d, a) * orientation(c, d, b) <= 0;
}

// Berechnet die Schnittpunkte der Strecken a-b und c-d. Enthält entweder
// keinen Punkt, den einzigen Schnittpunkt oder die Endpunkte der
// Schnittstrecke. operator<, min, max müssen noch geschrieben werden!
vector<pt> lineSegmentIntersection(pt a, pt b, pt c, pt d) {
	vector<pt> result;
	if (orientation(a, b, c) == 0 && orientation(a, b, d) == 0 &&
			orientation(c, d, a) == 0 && orientation(c, d, b) == 0) {
		pt minAB = min(a, b), maxAB = max(a, b);
		pt minCD = min(c, d), maxCD = max(c, d);
		if (minAB < minCD && maxAB < minCD) return result;
		if (minCD < minAB && maxCD < minAB) return result;
		pt start = max(minAB, minCD), end = min(maxAB, maxCD);
		result.push_back(start);
		if (start != end) result.push_back(end); 
		return result;
	}
	double x1 = real(b) - real(a), y1 = imag(b) - imag(a);
	double x2 = real(d) - real(c), y2 = imag(d) - imag(c);
	double u1 = (-y1 * (real(a) - real(c)) + x1 * (imag(a) - imag(c))) /
			(-x2 * y1 + x1 * y2);
	double u2 = (x2 * (imag(a) - imag(c)) - y2 * (real(a) - real(c))) /
			(-x2 * y1 + x1 * y2);
	if (u1 >= 0 && u1 <= 1 && u2 >= 0 && u2 <= 1) {
		double x = real(a) + u2 * x1, y = imag(a) + u2 * y1;
		result.push_back(pt(x, y));
	}
	return result;
}

// Entfernung von Punkt p zur Gearden durch a-b.
double distToLine(pt a, pt b, pt p) {
	return abs(cross(p - a, b - a)) / abs(b - a);
}

// Liegt p auf der Geraden a-b?
bool pointOnLine(pt a, pt b, pt p) {
	return orientation(a, b, p) == 0;
}

// Liegt p auf der Strecke a-b?
bool pointOnLineSegment(pt a, pt b, pt p) {
	if (orientation(a, b, p) != 0) return false;
	return real(p) >= min(real(a), real(b)) &&
				 real(p) <= max(real(a), real(b)) &&
				 imag(p) >= min(imag(a), imag(b)) &&
				 imag(p) <= max(imag(a), imag(b));
}

// Entfernung von Punkt p zur Strecke a-b.
double distToSegment(pt a, pt b, pt p) {
  if (a == b) return abs(p - a);
  double segLength = abs(a - b);
	double u = ((real(p) - real(a)) * (real(b) - real(a)) +
			(imag(p) - imag(a)) * (imag(b) - imag(a))) /
			(segLength * segLength);
  pt projection(real(a) + u * (real(b) - real(a)),
  		imag(a) + u * (imag(b) - imag(a)));
  double projectionDist = abs(p - projection);
  if (!pointOnLineSegment(a, b, projection)) projectionDist = 1e30;
  return min(projectionDist, min(abs(p - a), abs(p - b)));
}

// Kürzeste Entfernung zwischen den Strecken a-b und c-d.
double distBetweenSegments(pt a, pt b, pt c, pt d) {
	if (lineSegmentIntersection(a, b, c, d)) return 0.0;
	double result = distToSegment(a, b, c);
	result = min(result, distToSegment(a, b, d));
	result = min(result, distToSegment(c, d, a));
	return min(result, distToSegment(c, d, b));
}

// Liegt d in der gleichen Ebene wie a, b, und c?
bool isCoplanar(pt a, pt b, pt c, pt d) {
	return abs((b - a) * (c - a) * (d - a)) < EPSILON;
}

// Berechnet den Flächeninhalt eines Polygons (nicht selbstschneidend).
// Punkte gegen den Uhrzeigersinn: positiv, sonst negativ.
double areaOfPolygon(vector<pt> &polygon) { // Jeder Eckpunkt nur einmal.
	double res = 0; int n = polygon.size();
	for (int i = 0; i < n; i++)
		res += real(polygon[i]) * imag(polygon[(i + 1) % n]) -
					 real(polygon[(i + 1) % n]) * imag(polygon[i]);
	return 0.5 * res;
}

// Schneiden sich (p1, p2) und (p3, p4) (gegenüberliegende Ecken).
bool rectIntersection(pt p1, pt p2, pt p3, pt p4) {
	double minx12=min(real(p1), real(p2)), maxx12=max(real(p1), real(p2));
	double minx34=min(real(p3), real(p4)), maxx34=max(real(p3), real(p4));
	double miny12=min(imag(p1), imag(p2)), maxy12=max(imag(p1), imag(p2));
	double miny34=min(imag(p3), imag(p4)), maxy34=max(imag(p3), imag(p4));
	return (maxx12 >= minx34) && (maxx34 >= minx12) &&
				 (maxy12 >= miny34) && (maxy34 >= miny12);
}

// Testet, ob ein Punkt im Polygon liegt (beliebige Polygone).
bool pointInPolygon(pt p, vector<pt> &polygon) { // Punkte nur einmal.
	pt rayEnd = p + pt(1, 1000000);
	int counter = 0, n = polygon.size();
	for (int i = 0; i < n; i++) {
		pt start = polygon[i], end = polygon[(i + 1) % n];
		if (lineSegmentIntersection(p, rayEnd, start, end)) counter++;
	}
	return counter & 1;
}

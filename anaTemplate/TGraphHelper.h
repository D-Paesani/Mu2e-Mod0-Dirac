

int fillGraph(TGraphErrors *g, double x, double y, double ex, double ey) {

    g->AddPoint(x, y);
    int n = g->GetN();
    g->SetPointError(n-1, ex, ey);
    return n;
    
}


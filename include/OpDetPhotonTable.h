//
// File: OpDetPhotonTable.h
//
// Author: Hunter Sullivan
//
// Discription:
//

#ifndef OPDETPHOTONTABLE_HH
#define OPDETPHOTONTABLE_HH

#include <vector>
#include <map>

namespace majorana {

class Photon
{
  public:
    Photon(const std::vector<float>& photonVertex);
    ~Photon();

    const std::vector<float>& Vertex() const { return fVertex; };
    
  private:
    std::vector<float> fVertex;
};

class OpDetPhotonTable
{
  using PhotonsDetected = std::map<unsigned, std::vector<Photon>>;

  public:
    static OpDetPhotonTable* Instance();
    ~OpDetPhotonTable();

    void AddPhoton(const unsigned& opchannel, const Photon& photon);
    inline void Reset() { fPhotonsDetected.clear(); fNPhotonsAbs=0; Initialize(); };
    inline void IncPhotonsAbs() { fNPhotonsAbs++; };
    void Print();
    void Initialize();

    const PhotonsDetected& GetPhotonsDetected() const { return fPhotonsDetected; };
    const unsigned         GetNPhotonsAbsorbed() const { return fNPhotonsAbs; };

  private:
    OpDetPhotonTable();
    static OpDetPhotonTable* instance;

    PhotonsDetected fPhotonsDetected;
    unsigned        fNPhotonsAbs;
};
}
#endif

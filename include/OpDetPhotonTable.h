/**
 * @file OpDetPhotonTable.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton table for holding the detected photons.
 * @date 07-04-2019
 * 
 */

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
    std::vector<float> fVertex; ///< The photon emission point
};

class OpDetPhotonTable
{
  using PhotonsDetected = std::map<unsigned, std::vector<Photon>>;

  public:
    static OpDetPhotonTable* Instance();
    static OpDetPhotonTable* CreateInstance();
    ~OpDetPhotonTable();

    /**
     * @brief Add photon to table.
     * 
     * @param opchannel Optical chanel that detected the photon.
     * @param photon The photon object.
     */
    void AddPhoton(const unsigned& opchannel, const Photon& photon);

    inline void Reset() { fPhotonsDetected.clear(); fNPhotonsAbs=0; Initialize(); };
    inline void IncPhotonsAbs() { fNPhotonsAbs++; };

    /**
     * @brief Print contents of table.
     * 
     */
    void Print();

    /**
     * @brief Initialize our table.
     * 
     */
    void Initialize();

    const PhotonsDetected& GetPhotonsDetected() const { return fPhotonsDetected; };
    const unsigned         GetNPhotonsAbsorbed() const { return fNPhotonsAbs; };

  private:
    OpDetPhotonTable();
    static OpDetPhotonTable* instance;

    PhotonsDetected fPhotonsDetected; ///< Map between the channel ID and the number of photons detected
    unsigned        fNPhotonsAbs;     ///< Number of photons absorbed 
};
}
#endif

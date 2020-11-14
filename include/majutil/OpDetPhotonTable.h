/**
 * @file OpDetPhotonTable.h
 * @author H. Sullivan (hsulliva@fnal.gov)
 * @brief Singleton table for holding the detected photons.
 * @date 07-04-2019
 * 
 */

#ifndef MAJUTIL_OPDETPHOTONTABLE_HH
#define MAJUTIL_OPDETPHOTONTABLE_HH
#include <iostream>
#include <vector>
#include <map>

namespace majutil {

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
    static OpDetPhotonTable* CreateInstance(const size_t& nMPPCs);
    ~OpDetPhotonTable();
    void PrintParticleCounts(){

        std::cout<<"------------SecndParticleCounts-----"<<std::endl;
        std::cout<<"Photons : "<<fProPhotonE.size()<<std::endl;
        std::cout<<"Electrons : "<<fProElectronE.size()<<std::endl;
        std::cout<<"Neutrons : "<<fProNeutronE.size()<<std::endl;
        std::cout<<"Alphas : "<<fProAlphaE.size()<<std::endl;

    };

    /**
     * @brief Add photon to table.
     * 
     * @param opchannel Optical chanel that detected the photon.
     * @param photon The photon object.
     */
    void AddPhoton(const unsigned& opchannel, const Photon& photon);

    inline void Reset() {
        fPhotonsDetected.clear();
        fNPhotonsAbs=0; Initialize();
        fProPhotonE.clear();
        fProElectronE.clear();
        fProNeutronE.clear();
        fProAlphaE.clear();

    };
    inline void IncPhotonsAbs() { fNPhotonsAbs++; };

    /**
     * @brief Dump contents of table.
     * 
     */
    void Dump();

    /**
     * @brief Initialize our table.
     * 
     */
    void Initialize();

    const PhotonsDetected& GetPhotonsDetected() const { return fPhotonsDetected; };
    const unsigned         GetNPhotonsAbsorbed() const { return fNPhotonsAbs; };
    void AddElectronE(double E){fProElectronE.push_back(E);};
    void AddPhotonE(double E){fProPhotonE.push_back(E);};
    void AddNeutronE(double E){fProNeutronE.push_back(E);};
    void AddAlphaE(double E){fProAlphaE.push_back(E);};
    std::vector<double>  GetProPhotonEs  (){ return fProPhotonE;};
    std::vector<double> GetProElectronEs  (){ return fProElectronE;};
    std::vector<double> GetProNeutronEs  (){ return fProNeutronE;};
    std::vector<double> GetProAlphaEs  (){ return fProAlphaE;};


private:
    OpDetPhotonTable(const size_t& nMPPCs);
    static OpDetPhotonTable* instance;

    PhotonsDetected fPhotonsDetected; ///< Map between the channel ID and the number of photons detected
    unsigned        fNPhotonsAbs;     ///< Number of photons absorbed 
    unsigned        fNOpDet;          ///< Number of detectors used in table

    //Energies of the Produced Photons and Electrons
    std::vector<double>   fProPhotonE;
    std::vector<double>   fProElectronE;
    std::vector<double>   fProNeutronE;
    std::vector<double>   fProAlphaE;
};
}
#endif

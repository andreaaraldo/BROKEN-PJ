/*
 * ccnSim is a scalable chunk-level simulator for Content Centric
 * Networks (CCN), that we developed in the context of ANR Connect
 * (http://www.anr-connect.org/)
 *
 * People:
 *    Giuseppe Rossini (lead developer, mailto giuseppe.rossini@enst.fr)
 *    Raffaele Chiocchetti (developer, mailto raffaele.chiocchetti@gmail.com)
 *    Dario Rossi (occasional debugger, mailto dario.rossi@enst.fr)
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef REPRESENTATIONAWARE_DISTRIBUTION_H
#define REPRESENTATIONAWARE_DISTRIBUTION_H
#include <omnetpp.h>
#include "ccnsim.h"
#include "content_distribution.h"
#include "zipf.h"


using namespace std;

//<aa>
class RepresentationAwareContentDistribution : public content_distribution{
	public:
		static const unsigned short get_number_of_representations();
		static const double get_bitrate(unsigned short representation);
		static const unsigned get_storage_space_of_representation(unsigned short representation);

		static const unsigned get_storage_space_of_chunk(chunk_t chunk_id);	// Check what is the representation of 
																			// the chunk and returns the required
																			// storage space

		static const unsigned short get_representation_number(chunk_t chunk_id);
		static const representation_mask_t set_bit_to_zero(representation_mask_t mask, unsigned short position);
};
//</aa>
#endif


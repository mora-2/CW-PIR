#include <iostream>
#include "params.h"
#include "PIRClient.h"
#include "PIRServer.h"

using namespace std;

void cwPIR(
    uint64_t hamming_weight,
    EqualityType eq_type,
    uint64_t log_poly_mod_degree,
    uint64_t prime_bitlength,
    string csv_filepath,
    string keyword_column_name,
    string value_column_name,
    uint64_t number_of_keywords,
    uint64_t max_per_row_payload_byte_size,
    uint64_t keyword_bitlength,
    bool verbose,
    string write_path,
    uint64_t num_threads)
{
    if (keyword_bitlength <= 0)
    {
        keyword_bitlength = (uint64_t)(ceil(log2(number_of_keywords)));
    }

    // setup database
    Database db(number_of_keywords, max_per_row_payload_byte_size * 2, keyword_bitlength, csv_filepath, keyword_column_name, value_column_name);

    // params setup
    ServerParameters *s_param = new ServerParameters(log_poly_mod_degree, prime_bitlength, eq_type, hamming_weight, &db, keyword_bitlength, num_threads);
    s_param->write_path = write_path;
    // server send
    // s_param->encoding_size, s_param->num_output_ciphers
    ClientParameters *c_param = new ClientParameters(log_poly_mod_degree, prime_bitlength, eq_type, hamming_weight, s_param->encoding_size, s_param->num_output_ciphers);

    PIRClient client;
    PIRServer server;

    // crypto setup
    client.setup_crypto(c_param->log_poly_mod_degree, c_param->prime_bitlength, verbose); // client
    // server receive
    // client.parms_stream
    server.initialize_params_with_input(client.parms_stream); // server

    // db -> plaintext
    server.prep_db(s_param, verbose); // server

    // save rlk,gal keys
    // server receive client.data_stream
    client.send_evaluation_keys();

    /*-------------------------------------------------------------------------*/
    /*                                online                                   */
    /*-------------------------------------------------------------------------*/

    // query make
    // server receive encrypted query
    uint64_t query = 0; // key-index of query
    client.submit_PIR_query_with_params(query, c_param, verbose);

    // answering
    // server send answer
    server.respond_pir_query(client.data_stream, s_param, verbose);

    // reconstruct
    vector<Plaintext> response_pts = client.load_and_decrypt(c_param);
    vector<uint64_t> response_vec = client.pts_to_vec(response_pts, c_param->num_output_ciphers, c_param->poly_mod_degree);

    // timings
    c_param->metrics_["comm_relin"] = client.comm_relin;
    c_param->metrics_["comm_gals"] = client.comm_gals;
    c_param->metrics_["comm_query"] = client.comm_query;
    c_param->metrics_["comm_response"] = client.comm_response;

    s_param->add_parameters_to_metrics();

    s_param->print_db_specific_parameters();
    // Printing timings
    if (verbose)
    {
        cout << "------------------------ Timing ----------------------------------------------" << endl;
        cout << "\tDatabase Prep   : " << setw(10) << s_param->metrics_["time_prep"] / 1000 << " ms" << endl;
        cout << "\tExpansion time: : " << setw(10) << s_param->metrics_["time_expansion"] / 1000 << " ms" << endl;
        cout << "\tSel. Vec. Calc. : " << setw(10) << s_param->metrics_["time_sel_vec"] / 1000 << " ms" << endl;
        cout << "\tInner Product   : " << setw(10) << s_param->metrics_["time_inner_prod"] / 1000 << " ms" << endl;
        cout << "\tTotal Server    : " << setw(10) << s_param->metrics_["time_server_total"] / 1000 << " ms" << endl;
        cout << "--------------------- Communication ------------------------------------------" << endl
             << "\tData Independant: " << c_param->metrics_["comm_relin"] / 1000 << " KB (Relin keys) + " << c_param->metrics_["comm_gals"] / 1000 << " KB (Gal Keys)" << endl
             << "\tData Dependant: " << c_param->metrics_["comm_query"] / 1000 << " KB (Query) + " << c_param->metrics_["comm_response"] / 1000 << " KB (Reponse)" << endl
             << endl
             << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << endl;
    }

    // validate
    bool check = client.validate_response(s_param, query, response_vec, verbose);
    if (!check)
    {
        cout << "Noise Level: " << *min_element(begin(client.final_noise_level), end(client.final_noise_level)) << endl;
    }
    c_param->metrics_["valid_response"] = check;
}

int main(int argc, char *argv[])
{
    // crypto
    uint64_t log_poly_mod_degree = 13;
    uint64_t prime_bitlength = 21;

    // database
    string csv_filepath = "/home/yuance/Work/Encryption/PIR/code/PIR/CWPIR/data/data.csv";
    string keyword_column_name = "Child's First Name";
    string value_column_name = "Rank";
    uint64_t number_of_keywords = 57582;
    uint64_t keyword_bitlength = 0;
    uint64_t max_per_row_payload_byte_size = 4; // entry size

    // constant weight code
    EqualityType eq_type = Constant_Weight;
    uint64_t hamming_weight = 2;

    uint64_t num_threads = 32;
    bool verbose = true;
    string write_path = "";

    // This is a defalut value which will fill each ciphertext
    if (max_per_row_payload_byte_size <= 0)
        max_per_row_payload_byte_size = (1 << log_poly_mod_degree) * 5 / 2;

    // pir
    cwPIR(
        hamming_weight,
        eq_type,
        log_poly_mod_degree,
        prime_bitlength,
        csv_filepath,
        keyword_column_name,
        value_column_name,
        number_of_keywords,
        max_per_row_payload_byte_size,
        keyword_bitlength,
        verbose,
        write_path,
        num_threads);
    return 0;
}
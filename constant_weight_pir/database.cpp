#include "database.h"

Database::Database(uint64_t __num_keywords, uint64_t __max_item_hex_length, uint64_t __MAX_KEYWORD_BITLENGTH, string csv_filepath, string keyword_column_name, string value_column_name)
{
    this->num_keywords = __num_keywords;
    this->max_item_hex_length = __max_item_hex_length;
    if (__num_keywords <= (1 << __MAX_KEYWORD_BITLENGTH))
    {
        this->MAX_KEYWORD_BITLENGTH = __MAX_KEYWORD_BITLENGTH;
    }
    else
    {
        this->MAX_KEYWORD_BITLENGTH = ceil(log2(this->num_keywords));
        cout << "keyword bitlength too short, changing to " << this->MAX_KEYWORD_BITLENGTH << endl;
    }

    this->csv_filepath = csv_filepath;
    this->keyword_column_name = keyword_column_name;
    this->value_column_name = value_column_name;

    // this->generate_some_hex_data();
    this->read_data_from_csv();
}

void Database::read_data_from_csv()
{
    std::string filename = this->csv_filepath; // 替换为你的 CSV 文件路径
    std::string k_col_name = this->keyword_column_name;
    std::string v_col_name = this->value_column_name;

    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open the csv file." << std::endl;
        return;
    }

    // 读取第一行，即列名
    std::string header;
    std::getline(file, header);

    // 切割列名
    std::vector<std::string> column_names = strsplit(header, ',');

    // 找到 k_col_name 所在的列
    auto it = std::find(column_names.begin(), column_names.end(), k_col_name);
    if (it == column_names.end())
    {
        std::cerr << "Error: Column 'name' not found." << std::endl;
        return;
    }
    // 获取 k_col_name 列的索引
    size_t k_col_name_index = std::distance(column_names.begin(), it);

    // 找到 v_col_name 所在的列
    it = std::find(column_names.begin(), column_names.end(), v_col_name);
    if (it == column_names.end())
    {
        std::cerr << "Error: Column 'name' not found." << std::endl;
        return;
    }
    // 获取 v_col_name 列的索引
    size_t v_col_name_index = std::distance(column_names.begin(), it);

    // 读取数据到 vector<string>
    std::vector<std::string> keys;

    std::string line;
    uint64_t index = 0;
    while (std::getline(file, line))
    {
        std::vector<std::string> values = strsplit(line, ',');
        if (values.size() > max(k_col_name_index, v_col_name_index))
        {
            keys.push_back(values[k_col_name_index]);
            this->keywords.push_back(index++);
            this->data_hex_raw.push_back(values[v_col_name_index]);
        }
    }
}

void Database::generate_some_hex_data()
{
    srand(time(NULL));
    uint64_t _step = pow(2, this->MAX_KEYWORD_BITLENGTH - log2(this->num_keywords));
    for (int i = 0; i < this->num_keywords; i++)
    {

        uint64_t __new_keyword = i * _step;
        string __new_data_hex = gen_random_hex(this->max_item_hex_length, i);
        this->keywords.push_back(__new_keyword);
        this->data_hex_raw.push_back(__new_data_hex);
    }
}

uint64_t Database::get_random_query()
{
    srand(time(NULL));
    uint64_t _rand_index = rand() % this->num_keywords;
    return this->keywords[_rand_index];
}

uint64_t Database::get_index_of_keyword(uint64_t keyword)
{
    for (int i = 0; i < num_keywords; i++)
    {
        if (this->keywords[i] == keyword)
        {
            return i;
        }
    }
    return 0; // TODO: return something better in this case
}

vector<string> Database::get_hex_data_by_keyword(uint64_t keyword, uint64_t coefficient_hexlength)
{
    vector<string> _ans;
    uint64_t keyword_index = get_index_of_keyword(keyword);
    uint64_t __number_of_coeffs = ceil((float)this->data_hex_raw[keyword_index].length() / coefficient_hexlength);
    for (int i = 0; i < __number_of_coeffs; i++)
    {
        _ans.push_back(this->data_hex_raw[keyword_index].substr(i * coefficient_hexlength, coefficient_hexlength));
    }
    return _ans;
}

vector<string> Database::get_hex_data_by_keyword_index(uint64_t keyword_index, uint64_t coefficient_hexlength)
{
    vector<string> _ans;
    uint64_t __number_of_coeffs = ceil((float)this->data_hex_raw[keyword_index].length() / coefficient_hexlength);
    for (int i = 0; i < __number_of_coeffs; i++)
    {
        _ans.push_back(this->data_hex_raw[keyword_index].substr(i * coefficient_hexlength, coefficient_hexlength));
    }
    return _ans;
}

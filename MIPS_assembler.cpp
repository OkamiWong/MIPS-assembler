#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

// Storing the raw information about all instructions, which will be
// deserialized later in the program.
const string INSTRUCTIONS[] = {
    "div rs rt",       "divu rs rt",      "mult rs rt ",     "multu rs rt",
    "madd rs rt",      "maddu rs rt",     "msub rs rt",      "msub rs rt",
    "teq rs rt",       "tne rs rt",       "tge rs rt",       "tgeu rs rt",
    "tlt rs rt",       "tltu rs rt",      "mfhi rd",         "mflo rd",
    "jr rs",           "mthi rs",         "mtlo rs",         "addi rt rs imm",
    "addiu rt rs imm", "andi rt rs imm",  "ori rt rs imm",   "xori rt rs imm",
    "lui rt imm",      "slti rt rs imm",  "sltiu rt rs imm", "teqi rs imm",
    "tnei rs imm",     "tgei rs imm",     "tgeiu rs imm",    "tlti rs imm",
    "tltiu rs imm",    "beq rs rt label", "bgez rs label",   "bgezal rs label",
    "bgtz rs label",   "blez rs label",   "bltzal rs label", "bltz rs label",
    "bne rs rt label", "lb rt address",   "lbu rt address",  "lh rt address",
    "lhu rt address",  "lw rt address",   "lwl rt address",  "lwr rt address",
    "ll rt address",   "sb rt address",   "sh rt address",   "sw rt address",
    "swl rt address",  "swr rt address",  "sc rt address",   "add rd rs rt",
    "addu rd rs rt",   "and rd rs rt",    "clo rd rs",       "clz rd rs",
    "mul rd rs rt",    "nor rd rs rt",    "or rd rs rt",     "sll rd rt shamt",
    "sllv rd rt rs",   "sra rd rt shamt", "srav rd rt rs",   "srl rd rt shamt",
    "srlv rd rt rs",   "sub rd rs rt",    "subu rd rs rt",   "xor rd rs rt",
    "slt rd rs rt",    "sltu rd rs rt",   "jalr rs rd",      "j target",
    "jal target"};

// Storing all the raw information about how to translate each instruction.
// Each instruction and its translation are stored at the same index.
const string PLACE_HOLDERS[] = {"0 rs rt 0 0x1a",
                                "0 rs rt 0 0x1b",
                                "0 rs rt 0 0x18",
                                "0 rs rt 0 0x19",
                                "0x1c rs rt 0 0",
                                "0x1c rs rt 0 1",
                                "0x1c rs rt 0 4",
                                "0x1c rs rt 0 5",
                                "0 rs rt 0 0x34",
                                "0 rs rt 0 0x36",
                                "0 rs rt 0 0x30",
                                "0 rs rt 0 0x31",
                                "0 rs rt 0 0x32",
                                "0 rs rt 0 0x33",
                                "0 0 0 rd 0 0x10",
                                "0 0 0 rd 0 0x12",
                                "0 rs 0 8",
                                "0 rs 0 0x11",
                                "0 rs 0 0x13",
                                "8 rs rt imm",
                                "9 rs rt imm",
                                "0xc rs rt imm",
                                "0xd rs rt imm",
                                "0xe rs rt imm",
                                "0xf 0 rt imm",
                                "0xa rs rt imm",
                                "0xb rs rt imm",
                                "1 rs 0xc imm",
                                "1 rs 0xe imm",
                                "1 rs 8 imm",
                                "1 rs 9 imm",
                                "1 rs a imm",
                                "1 rs b imm",
                                "4 rs rt Offset",
                                "1 rs 1 Offset",
                                "1 rs 0x11 Offset",
                                "7 rs 0 Offset",
                                "6 rs 0 Offset",
                                "1 rs 0x10 Offset",
                                "1 rs 0 Offset",
                                "5 rs rt Offset",
                                "0x20 rs rt Offset",
                                "0x24 rs rt Offset",
                                "0x21 rs rt Offset",
                                "0x25 rs rt Offset",
                                "0x23 rs rt Offset",
                                "0x22 rs rt Offset",
                                "0x26 rs rt Offset",
                                "0x30 rs rt Offset",
                                "0x28 rs rt Offset",
                                "0x29 rs rt Offset",
                                "0x2b rs rt Offset",
                                "0x2a rs rt Offset",
                                "0x2e rs rt Offset",
                                "0x38 rs rt Offset",
                                "0 rs rt rd 0 0x20",
                                "0 rs rt rd 0 0x21",
                                "0 rs rt rd 0 0x24",
                                "0x1c rs 0 rd 0 0x21",
                                "0x1c rs 0 rd 0 0x20",
                                "0x1c rs rt rd 0 2",
                                "0 rs rt rd 0 0x27",
                                "0 rs rt rd 0 0x25",
                                "0 rs rt rd shamt 0",
                                "0 rs rt rd 0 4",
                                "0 rs rt rd shamt 3",
                                "0 rs rt rd 0 7",
                                "0 rs rt rd shamt 2",
                                "0 rs rt rd 0 6",
                                "0 rs rt rd 0 0x22",
                                "0 rs rt rd 0 0x23",
                                "0 rs rt rd 0 0x26",
                                "0 rs rt rd 0 0x2a",
                                "0 rs rt rd 0 0x2b",
                                "0 rs 0 rd 0 9",
                                "2 target",
                                "3 target"};

const int NUMBER_OF_INSTRUCTIONS = 77;

// Storing the length of fields of instructions.
const int PLACE_HOLDER_LENGTHS[] = {6, 5, 5, 5, 5, 6};

// The total length of an instruction.
const int INSTRUCTION_LENGTH = 32;

// The length of the immediate field in I-type instructions.
const int IMM_LENGTH = 16;

// Storing all the names of registers.
// The index of each name is their number in  MIPS.
const string reg_names[] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2",
                            "$a3",   "$t0", "$t1", "$t2", "$t3", "$t4", "$t5",
                            "$t6",   "$t7", "$s0", "$s1", "$s2", "$s3", "$s4",
                            "$s5",   "$s6", "$s7", "$t8", "$t9", "$k0", "$k1",
                            "$gp",   "$sp", "$fp", "$ra"};

// Storing the corresponding number of each register.
map<string, int> reg_name_to_int;

// Base struct to store deserialized instructions.
struct instruction {
    vector<string> params;        // names of parameters
    vector<string> place_holders; // constant values or references to parameters
};

// Link each deserialized instruction with its name.
map<string, instruction> name_to_instrutcion;

// Storing which instruction a label is pointing at.
// It should be noticed that the line number here starts from 0 and ignores all
// the blank lines.
map<string, int> label_to_line_number;

// Streams for file IO.
ifstream input_stream;
ofstream output_stream;

// Accepts a string and split it by tab and space and ignoring comments.
// Returns a vector containing the split strings.
vector<string> split_string(string raw_string) {
    vector<string> tokens;

    int token_start_index = -1; // Record the beginning of each token.
    for (int i = 0; i < raw_string.length(); i++) {
        char ch = raw_string[i];
        if (token_start_index == -1) {
            if (ch == '#')
                break;
            if (ch == ' ' || ch == '\t')
                continue;

            // A token begins.
            token_start_index = i;

        } else {
            if (ch == '#' || ch == ' ' || ch == '\t') {
                // A token ends. Store it.
                tokens.push_back(raw_string.substr(token_start_index,
                                                   i - token_start_index));
                token_start_index = -1;
                if (ch == '#')
                    break;
            }
        }
    }

    // Check whether we have stored the last token.
    if (token_start_index != -1)
        tokens.push_back(raw_string.substr(
            token_start_index, raw_string.length() - token_start_index));

    return tokens;
}

// A token is basically a string contains no spaces, tabs or '\n'.
// Read a new line and returns all the tokens in this line, ignoring comments.
// If there is no token in the line, return an empty vector.
vector<string> get_tokens_in_line() {
    vector<string> tokens;
    if (input_stream.eof())
        return tokens;

    string raw_line;
    while (input_stream.peek() == '\n')
        input_stream.get(); // Reading an empty line causes error in getline.
    if (input_stream.eof())
        return tokens;

    getline(input_stream, raw_line);

    return split_string(raw_line);
}

// Scanning labels in the source file and record their positions.
void scan_labels() {
    vector<string> tokens;
    string token;
    int line_number = -1; // Counting lines ignoring blank lines.
    while (true) {
        tokens = get_tokens_in_line();
        if (tokens.empty()) {
            // When there is no token in the line, the file may end or the line
            // is blank.
            if (input_stream.eof())
                break;
            else
                continue;
        }

        token = tokens[0];
        if (token[token.length() - 1] == ':') {
            token.pop_back(); // Remove the ':' at the end.
            label_to_line_number[token] = line_number + 1;
        }

        if (tokens.size() != 1) // Check whether there is only a label in the line.
            line_number++;
    }
}

// Read a hexadecimal number stored in a string s, and return it as a integer.
// Only accepts nonegative number.
int read_hex(string s) {
    int n = 0;

    if (s.length() == 1) {
        // First case: 0, 1, 2, a, b, c ,...
        if ('0' <= s[0] && s[0] <= '9')
            n = s[0] - '0';
        else
            n = s[0] - 'a' + 10;
    } else {
        // Second case: 0x123, 0xac, 0x1a1 ,...
        for (int i = 2; i < s.length(); ++i) {
            n *= 16;
            if ('0' <= s[i] && s[i] <= '9')
                n += s[i] - '0';
            else
                n += s[i] - 'a' + 10;
        }
    }

    return n;
}

// Accepts a name of a register, such as "$t0",
// returns the corresponding number of the register.
int read_reg(string s) { return reg_name_to_int[s]; }

// Accepts a name of a label,
// returns the position of the label.
int read_label(string s) { return label_to_line_number[s]; }

// Read a decimal number stored in a string s, and return it as a integer.
// Accepts both nonegative number and negative number.
int read_dec(string s) {
    int n = 0, sign = 1;
    int length = s.length();
    for (int i = 0; i < s.length(); i++) {
        if (s[i] == '-') {
            sign = -1;
            continue;
        }
        n *= 10;
        n += s[i] - '0';
    }
    return sign * n;
}

// Accepts a string s like "123($s0)".
// Stores the offset in (imm) and the number of the register in (reg).
void read_address(string s, int &imm, int &reg) {
    imm = 0;
    for (int i = 0; i < s.length(); ++i) {
        if (s[i] == '(') {
            if (i != 0)
                imm = read_dec(s.substr(0, i));
            reg = read_reg(s.substr(i + 1, s.length() - 1 - (i + 1)));
            return;
        }
    }
}

// Write a number in binary form. Length is provided by the caller.
// If n is negative then write the second-complement.
void write_binary(int n, int length) {
    string s;
    s.resize(length);
    while (length) {
        length--;
        s[length] = (char)((n & 1) + '0');
        n >>= 1;
    }
    output_stream << s;
}

// Program Counter
int PC = -4;

// Accepts tokens in an instruction and the offset of the beginning of the
// instruction. Then translate the instruction and write it.
// start_offset here is used for ignoring the label at the beginning of the
// line.
void translate_instruction(vector<string> tokens, int start_offset) {
    int &o = start_offset;
    instruction &ins = name_to_instrutcion[tokens[o]];

    // Remove the ',' in tokens.
    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i][tokens[i].length() - 1] == ',')
            tokens[i].pop_back();
    }

    // Store the paramters' value
    map<string, string> params;
    for (int i = 0; i < ins.params.size(); ++i)
        params[ins.params[i]] = tokens[o + i + 1];

    int remaining_length = INSTRUCTION_LENGTH;
    int current_field, current_length;

    // When we need to read an address like "123($s0)", the sequence of writing
    // is first write rs and then rt, which is different from the sequence of
    // reading: first read rt then read rs.
    // Therefore, it should be treated specially.
    if (ins.params[ins.params.size() - 1] == "address") {
        write_binary(read_hex(ins.place_holders[0]), PLACE_HOLDER_LENGTHS[0]);
        int imm, rs;
        read_address(params["address"], imm, rs);
        write_binary(rs, PLACE_HOLDER_LENGTHS[1]);
        write_binary(read_reg(params["rt"]), PLACE_HOLDER_LENGTHS[2]);
        write_binary(imm, IMM_LENGTH);
        output_stream << endl;
        return;
    }

    // Read and write in the same sequence.
    for (int i = 0; i < ins.place_holders.size(); ++i) {

        if (params.find(ins.place_holders[i]) != params.end()) {
            // When the name of the parameter exists in the place holders.

            // When the name of the parameter is "shamt", the value of it must
            // be a decimal value.
            if (ins.place_holders[i] == "shamt")
                current_field = read_dec(params["shamt"]);
            else if (ins.place_holders[i] == "target") {
                // Check the target is a label or an address.
                if (label_to_line_number.find(params["target"]) !=
                    label_to_line_number.end())
                    current_field = read_label(params["target"]);
                else
                    current_field = read_hex(params["target"]);
            } else {
                if (params[ins.place_holders[i]][0] == '$')
                    current_field = read_reg(params[ins.place_holders[i]]);
                else
                    current_field = read_dec(params[ins.place_holders[i]]);
            }

        } else if (ins.place_holders[i] == "Offset")
            current_field =
                (read_label(tokens[tokens.size() - 1]) * 4 - PC - 4) / 4;
        else if (ins.place_holders[i] == "rs")
            // When the rs value is ignored in the instruction
            current_field = 0;
        else
            // A constant value of the instruction
            current_field = read_hex(ins.place_holders[i]);

        // Check whether it is the last field
        if (i == ins.place_holders.size() - 1)
            current_length = remaining_length;
        else
            current_length = PLACE_HOLDER_LENGTHS[i];

        write_binary(current_field, current_length);
        remaining_length -= current_length;
    }

    output_stream << endl;
}

// If the line is blank, ignore it.
// If the line contains an instruction, translate it.
// Return false only when the end of the file is reached.
bool process_line() {
    vector<string> tokens = get_tokens_in_line();
    if (tokens.empty()) {
        if (input_stream.eof())
            return false;
        return true;
    }

    if (tokens.size() == 1)
        return true;

    PC += 4;

    string first_token = tokens[0];

    int start_offset = 0;

    if (first_token.find(':') != first_token.npos) {
        // Found a label (otherwise, a is the name of a instruction)
        start_offset = 1;
    }
    translate_instruction(tokens, start_offset);
    return true;
}

void assemble() {
    // Keep processing new line until reached the end of the file
    while (process_line())
        ;
}

void initialize_reg_name_to_int() {
    for (int i = 0; i < 32; i++)
        reg_name_to_int[reg_names[i]] = i;
}

// Deserialize instructions' definitions.
void initialize_instructions() {
    vector<string> params, place_holders;
    instruction *ins;
    for (int i = 0; i < NUMBER_OF_INSTRUCTIONS; ++i) {
        params = split_string(INSTRUCTIONS[i]);
        place_holders = split_string(PLACE_HOLDERS[i]);

        ins = new instruction();

        for (int i = 1; i < params.size(); ++i)
            ins->params.push_back(params[i]);
        for (int i = 0; i < place_holders.size(); ++i)
            ins->place_holders.push_back(place_holders[i]);

        name_to_instrutcion[params[0]] = *ins;
    }
}

int main(int argc, char **argv) {
    // Check whether the user provided the correct number of arguments.
    // There should be three arguments:
    //      [executable_assembler_file] [input_file_path] [output_file_path]
    if (argc != 3) {
        cout << "Wrong number of arguments!" << endl;
        return -1;
    }

    // Open the input file and output file
    input_stream.open(argv[1]);
    output_stream.open(argv[2]);

    scan_labels();

    input_stream.seekg(0, ios::beg);

    initialize_reg_name_to_int();
    initialize_instructions();

    // Start assembling.
    assemble();

    return 0;
}

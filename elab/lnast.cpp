
#include "lnast.hpp"

//------------- Language_neutral_ast member function start -----
Language_neutral_ast::Language_neutral_ast(std::string_view _buffer, Lnast_ntype_id ntype_top) : buffer(_buffer) {
  I(!buffer.empty());
  set_root(Lnast_node(ntype_top, 0, 0));
}


//------------- Lnast_parser member function start -------------

void Lnast_parser::elaborate(){
  lnast = std::make_unique<Language_neutral_ast>(get_buffer(), Lnast_ntype_top);
  build_statements(0);
}


void Lnast_parser::build_statements(Scope_id scope){
  auto tree_idx_sts  = lnast->add_child(lnast->get_root(), Lnast_node(Lnast_ntype_statement, scan_token(), scope));
  add_statement(tree_idx_sts, scope);
}

void Lnast_parser::add_statement(const Tree_index &tree_idx_sts, Scope_id cur_scope) {
  fmt::print("line:{}, statement:{}\n", line_num, scan_text());

  int line_tkcnt = 0;
  Token_entry    node_name;
  Lnast_ntype_id node_type;
  Token_entry    cfg_token_beg;
  Token_entry    cfg_token_end;
  Scope_id       token_scope;

  while(line_num == scan_calc_lineno()){
    if(scan_is_end()) return;

    line_tkcnt += 1;
    switch (line_tkcnt) {
      case CFG_NODE_NAME_POS:
        node_name = scan_token();  //must be a complete alnum
        break;
      case CFG_SCOPE_ID_POS:
        token_scope = process_scope(cur_scope); //recursive build subgraph here
        break;
      case CFG_TOKEN_POS_BEG:
        cfg_token_beg = scan_token(); //must be a complete alnum
        break;
      case CFG_TOKEN_POS_END:
        cfg_token_end = scan_token(); //must be a complete alnum
        break;
      case CFG_OP_POS_BEG:
        //note: no regulation after operator position
        operator_analysis(node_type, line_tkcnt);
        break;
      default: add_operator_subtree(node_type, line_tkcnt);
    }

    fmt::print("token:{}\n", scan_text());
    scan_next();
  }

  line_num += 1;
  add_statement(tree_idx_sts, cur_scope);
}

Scope_id Lnast_parser::process_scope(Scope_id cur_scope) {
  auto token_scope= (uint8_t)std::stoi(scan_text());
  if(token_scope == cur_scope){
    return token_scope;
  } else if (token_scope < cur_scope){
    for(int i = 0; i < CFG_SCOPE_ID_POS; i++) scan_prev();
    //going back to parent scope
    return token_scope;
  } else {//(token_scope > cur_scope)
    for(int i = 0; i < CFG_SCOPE_ID_POS; i++) scan_prev();
    add_subgraph(); //SH:FIXME: deal with subgraph later
    return token_scope;
  }
}

void Lnast_parser::add_subgraph() {
  ;
}

void Lnast_parser::operator_analysis(Lnast_ntype_id & node_type, int& line_tkcnt) {
  if (scan_is_token(Token_id_op)) { //deal with ()
    node_type = Lnast_ntype_tuple; // must be a tuple op
    I(scan_next_token_is(Token_id_cp));
    scan_next();
    line_tkcnt += 1;
  } else if (scan_is_token(Token_id_colon)) {
      if (scan_next_token_is(Token_id_colon)) { //deal with ::{
        node_type = Lnast_ntype_func_def;
        scan_next();
        I(scan_next_token_is(Token_id_ob)); //must be a function def op
        scan_next();
        line_tkcnt += 2;
      } else if (scan_next_token_is(Token_id_eq)) { //deal with :=
        node_type = Lnast_ntype_dp_assign;
        scan_next();
        line_tkcnt += 1;
      } else {
        node_type = Lnast_ntype_lable;
      }
  } else if (scan_is_token(Token_id_dot)) { //deal with .()
      if (scan_next_token_is(Token_id_op)){
        node_type = Lnast_ntype_func_call; // must be a function call op
        scan_next();
        I(scan_next_token_is(Token_id_cp)); //must be a function def op
        scan_next();
        line_tkcnt += 2;
      } else {
        node_type = Lnast_ntype_dot;
      }
  } else if (scan_is_token(Token_id_alnum) && scan_text() == "as") {
    node_type = Lnast_ntype_as;
  } else if (scan_is_token(Token_id_alnum) && scan_text() == "for") {
    node_type = Lnast_ntype_for;
  } else if (scan_is_token(Token_id_alnum) && scan_text() == "while") {
    node_type = Lnast_ntype_while;
  } else if (scan_is_token(Token_id_alnum) && scan_text() == "if") {
    node_type = Lnast_ntype_if;
  } else if (scan_is_token(Token_id_alnum) && scan_text() == "uif") {
    node_type = Lnast_ntype_uif;
  } else if (scan_is_token(Token_id_alnum) && scan_text() == "I") {
    node_type = Lnast_ntype_assert;
  } else if (scan_is_token(Token_id_eq)) {
    node_type = Lnast_ntype_pure_assign;
  } else if (scan_is_token(Token_id_and)) {
    node_type = Lnast_ntype_and;
  } else if (scan_is_token(Token_id_or)) {
    node_type = Lnast_ntype_or;
  } else if (scan_is_token(Token_id_xor)) {
    node_type = Lnast_ntype_xor;
  } else if (scan_is_token(Token_id_plus)) {
    node_type = Lnast_ntype_plus;
  } else if (scan_is_token(Token_id_minus)) {
    node_type = Lnast_ntype_minus;
  } else if (scan_is_token(Token_id_mult)) {
    node_type = Lnast_ntype_mult;
  } else if (scan_is_token(Token_id_div)) {
    node_type = Lnast_ntype_div;
  } else if (scan_is_token(Token_id_eq)) {
    node_type = Lnast_ntype_eq;
  } else if (scan_is_token(Token_id_le)) {
    node_type = Lnast_ntype_le;
  } else if (scan_is_token(Token_id_lt)) {
    node_type = Lnast_ntype_lt;
  } else if (scan_is_token(Token_id_ge)) {
    node_type = Lnast_ntype_ge;
  } else if (scan_is_token(Token_id_gt)) {
    node_type = Lnast_ntype_gt;
  } else {
    node_type = Lnast_ntype_invalid;
  }
}
void Lnast_parser::add_operator_subtree(Lnast_ntype_id & node_type, int& line_tkcnt) {
  ;
}

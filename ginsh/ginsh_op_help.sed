# Convert help for ginsh operators from man page to C source
/GINSH_OP_HELP_START/,/GINSH_OP_HELP_END/{

# .B lines contain the operator symbol
/.B/{

# extract operator symbol
s/.B \(.*\)/insert_help("operators","\1\\t"/
s/\\-/-/g
p

# next line contains description
n
s/^/"/
s/$/");/
p
}
}

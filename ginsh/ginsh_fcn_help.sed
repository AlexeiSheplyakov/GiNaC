# Convert help for ginsh functions from man page to C source
/GINSH_FCN_HELP_START/,/GINSH_FCN_HELP_END/{

# .BI lines contain the function synopsis
/\.BI/{

# extract function name
h
s/\.BI \(.*\)(.*/insert_help("\1",/p
g

# extract synopsis
s/"//g
s/ , /,/g
s/\.BI /"/
s/( /(/
s/ )/)"/
p
# handle multi-line synopsis
s/.br/);/p
}

# \- lines contain the function description
/\\-/{
s/"/'/g
s/\\-/" -/
s/$/"/
p
}

# .br lines start the next function
/.br/s//);/p
}

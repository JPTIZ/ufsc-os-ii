def c(m, e, n):
    return pow(m, e, n)


def cypher(msg, e, n):
    print(f'cyphering: "{msg}"')
    return c(msg[0], e, n)


if __name__ == '__main__':
    print(f'cypher: {cypher([30], 123, 321)}')
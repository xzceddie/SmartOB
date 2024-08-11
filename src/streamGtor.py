import click
import random

def gen_orders(num_strings: int):
    strings = []
    
    for i in range(num_strings):
        index = i
        random_val = random.randint(0, 1)
        int_val = random.randint(1, 10) * 50
        float_val = round(random.uniform(0.9, 2.9), 1)
        
        string_format = f"N {i} {random_val} {int_val} {float_val}"
        strings.append(string_format)
    
    return strings

def gen_orders_to(num_strings: int, file_path: str):
    strings = gen_orders(num_strings)
    with open(file_path, 'w') as f:
        for string in strings:
            f.write(string + '\n')
    print(f"[gen_orders_to] Generated {num_strings} orders to {file_path}")

@click.command()
@click.option('--num_strings', default=10, help='Number of strings to generate')
@click.option('--file_path', default='orders.txt', help='where to put the generated order strings')
def main(num_strings, file_path):
    gen_orders_to(num_strings, file_path)

if __name__ == "__main__":
    main()
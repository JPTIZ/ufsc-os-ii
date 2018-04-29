#include <utility/ostream.h>
#include <utility/diffie_hellman.h>

using namespace EPOS;

struct cipher {
	static const unsigned int KEY_SIZE = 16;
};

OStream cout;

int main()
{
	Diffie_Hellman<cipher> node_key = Diffie_Hellman<cipher>();
	Diffie_Hellman<cipher> cloud_key = Diffie_Hellman<cipher>();

	Diffie_Hellman<cipher>::Shared_Key node_shared_key = node_key.shared_key(cloud_key.public_key());
	Diffie_Hellman<cipher>::Shared_Key cloud_shared_key = cloud_key.shared_key(node_key.public_key());

	cout << node_shared_key << endl;
	cout << cloud_shared_key << endl;
	if (node_shared_key == cloud_shared_key)
		cout << "Success!" << endl;

	while (true)
		{}

    return 0;
}
#include "skel.h"

int interfaces[ROUTER_NUM_INTERFACES];
struct route_table_entry *rtable;
int rtable_size;

struct arp_entry *arp_table;
int arp_table_len;

/*
 Returns a pointer (eg. &rtable[i]) to the best matching route
 for the given dest_ip. Or NULL if there is no matching route.
*/
struct route_table_entry *get_best_route(__u32 dest_ip) {
	/* TODO 1: Implement the function */
	for (int i = 0; i < rtable_size; i++) {
		if ((dest_ip & rtable[i].mask) == rtable[i].prefix) {
			return &rtable[i];
		}
	}
	return NULL;
}

/*
 Returns a pointer (eg. &arp_table[i]) to the best matching ARP entry.
 for the given dest_ip or NULL if there is no matching entry.
*/
struct arp_entry *get_arp_entry(__u32 ip) {
    /* TODO 2: Implement */
	for (int i = 0; i < arp_table_len; i++) {
		if (arp_table[i].ip == ip) {
			return &arp_table[i];
		}
	}
    return NULL;
}

int cmpfunc(const void *a, const void *b) {
	struct route_table_entry* first_entry = (struct route_table_entry*) a;
	struct route_table_entry* second_entry = (struct route_table_entry*) b;
	return second_entry->mask - first_entry->mask;
}

int main(int argc, char *argv[])
{
	msg m;
	int rc;

	init();
	rtable = malloc(sizeof(struct route_table_entry) * 100);
	arp_table = malloc(sizeof(struct  arp_entry) * 100);
	DIE(rtable == NULL, "memory");
	rtable_size = read_rtable(rtable);
	parse_arp_table();
	qsort(rtable, rtable_size, sizeof(struct route_table_entry), cmpfunc);
	for (int i = 0; i < rtable_size; i++) {
		printf("%d\n", rtable[i].mask);
	}
	/* Students will write code here */

	while (1) {
		rc = get_packet(&m);
		DIE(rc < 0, "get_message");
		struct ether_header *eth_hdr = (struct ether_header *)m.payload;
		struct iphdr *ip_hdr = (struct iphdr *)(m.payload + sizeof(struct ether_header));
		/* TODO 3: Check the checksum */
		uint16_t aux = ip_hdr->check;
		ip_hdr->check = 0;
		if (ip_checksum(ip_hdr, sizeof(struct iphdr)) != aux) {
			continue;
		}
		/* TODO 4: Check TTL >= 1 */
		if (ip_hdr->ttl < 1) {
			continue;
		}
		/* TODO 5: Find best matching route (using the function you wrote at TODO 1) */
		struct route_table_entry* best_route = get_best_route(ip_hdr->daddr);
		if (best_route == NULL) {
			continue;
		}
		/* TODO 6: Update TTL and recalculate the checksum */
		ip_hdr->ttl--;
		ip_hdr->check = ip_checksum(ip_hdr, sizeof(ip_hdr));
		/* TODO 7: Find matching ARP entry and update Ethernet addresses */
		struct arp_entry *matching_arp = get_arp_entry(ip_hdr->daddr);
		memcpy(eth_hdr->ether_dhost, matching_arp->mac, sizeof(matching_arp->mac));
		get_interface_mac(best_route->interface, eth_hdr->ether_shost);
		/* TODO 8: Forward the pachet to best_route->interface */
		send_packet(best_route->interface, &m);
	}
}

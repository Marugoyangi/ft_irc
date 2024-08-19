#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <vector>
# include <iostream>

class Channel
{
	private :
		std::string _channel_name;
		std::vector<int> _fdlist;

	public :
		Channel();
		~Channel();
		Channel &operator=(const Channel &other);
		Channel(const Channel &other);
};

#endif
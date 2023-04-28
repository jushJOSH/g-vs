#pragma once

#include <string>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

struct HLSConfig {
    HLSConfig(const std::string &playlistFolder, const std::string &playlistId, int targetDuration, int playlistLenght, int bias = 6)
    : target_duration(targetDuration), playlist_length(playlistLenght), bias(bias)
    {
        using boost::str;
        using boost::format;
        
        playlist_folder = str(format("%s/%s") % playlistFolder % playlistId);
        playlist_root = str(format("static/%s") % playlistId);
        segment_loc = str(format("%s/segment\%\%05d.ts") % playlist_folder);
        playlist_loc = str(format("%s/playlist.m3u8") % playlist_folder);
    }

    std::string playlist_folder; // folder for playlist and its segments
    std::string playlist_root; // playlist root (affects only inside playlist, not affect location)
    std::string playlist_loc; // playlist location
    std::string segment_loc; // new segment location
    int target_duration; // segment lenght in seconds
    int playlist_length; // segment quantity
    int bias; // offset in seconds
};
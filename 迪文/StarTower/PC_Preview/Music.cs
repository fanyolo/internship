using System;using System.IO;using System.Media;using System.Diagnostics;
// Application-local PCM gain: never changes the user's Windows master volume.
sealed class Music:IDisposable {
 readonly byte[][] tracks=new byte[4][]; readonly Stopwatch clock=new Stopwatch();
 readonly string errorPath; SoundPlayer player; MemoryStream stream; int track=-1,volume=-1; bool failed;
 public Music(string root){errorPath=Path.Combine(root,"PC_Preview","audio_error.log");for(int i=0;i<4;i++){tracks[i]=File.ReadAllBytes(Path.Combine(root,"Assets","50_Audio",i.ToString("D2")+".wav"));Validate(tracks[i]);}}
 public static void Validate(byte[] wav){
  if(wav.Length<44||System.Text.Encoding.ASCII.GetString(wav,0,4)!="RIFF"||System.Text.Encoding.ASCII.GetString(wav,8,8)!="WAVEfmt "||BitConverter.ToInt32(wav,16)!=16||BitConverter.ToInt16(wav,20)!=1||BitConverter.ToInt16(wav,22)!=1||BitConverter.ToInt32(wav,24)!=32000||BitConverter.ToInt16(wav,34)!=16||System.Text.Encoding.ASCII.GetString(wav,36,4)!="data"||BitConverter.ToInt32(wav,40)!=wav.Length-44)throw new IOException("Expected PCM16 mono 32000 Hz WAV");
 }
 public int Playing{get{return !failed&&track>=0&&volume>0&&clock.IsRunning&&clock.Elapsed.TotalSeconds<(tracks[track].Length-44)/64000.0?1:0;}}
 void Stop(){if(player!=null){player.Stop();player.Dispose();player=null;}if(stream!=null){stream.Dispose();stream=null;}}
 void StartAt(int offset){
  Stop();byte[] source=tracks[track];offset=Math.Min(source.Length-44,Math.Max(0,offset))&~1;
  int length=source.Length-44-offset;if(length==0)return;byte[] wav=new byte[44+length];Buffer.BlockCopy(source,0,wav,0,44);Buffer.BlockCopy(BitConverter.GetBytes(wav.Length-8),0,wav,4,4);Buffer.BlockCopy(BitConverter.GetBytes(length),0,wav,40,4);
  for(int i=0;i<length;i+=2){short sample=BitConverter.ToInt16(source,44+offset+i);short scaled=(short)(sample*volume/64);wav[44+i]=(byte)scaled;wav[45+i]=(byte)(scaled>>8);}
  stream=new MemoryStream(wav,false);player=new SoundPlayer(stream);player.Load();player.Play();
 }
 public void Update(ushort command){if(failed)return;int nextVolume=command&127,nextTrack=(command>>8)&63;bool play=(command&32768)!=0;
  try{if(nextVolume==0){Stop();clock.Reset();track=-1;volume=0;return;}
   if(play){if(nextTrack>=4)throw new IOException("Invalid music track");track=nextTrack;volume=nextVolume;StartAt(0);clock.Restart();}
   else if(nextVolume!=volume){volume=nextVolume;if(Playing!=0)StartAt((int)(clock.Elapsed.TotalSeconds*32000)*2);}
  }catch(Exception e){failed=true;Stop();File.WriteAllText(errorPath,e.ToString());}
 }
 public void Dispose(){Stop();clock.Stop();}
}

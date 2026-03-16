import torch
import torchaudio


def convert_to_mfcc(*, waveform, sample_rate, n_mfcc):
    mfcc_transform = torchaudio.transforms.MFCC(
        sample_rate=sample_rate, 
        n_mfcc=n_mfcc,
        melkwargs={"n_fft": 256, "n_mels": 40, "center": False},
    )
    delta_transform = torchaudio.transforms.ComputeDeltas()
    
    # 2. Compute the features
    mfcc = mfcc_transform(waveform)              # Static spectral envelope
    delta = delta_transform(mfcc)                # Velocity (1st derivative)
    delta_delta = delta_transform(delta)         # Acceleration (2nd derivative)
    
    # 3. Concatenate along the MFCC feature dimension (dim=-2)
    # If n_mfcc is 40, the new feature dimension will be 120 (40 + 40 + 40)
    combined_features = torch.cat((mfcc, delta, delta_delta), dim=-2)
    
    # 4. Pool across the time dimension (dim=-1) to capture more than just the average
    # Since audio is 5 seconds but the words are short, standard deviation and max
    # pooling help capture the actual signal variation instead of just the washed-out mean.
    features_mean = combined_features.mean(dim=-1)
    features_std = combined_features.std(dim=-1)
    features_max = combined_features.max(dim=-1).values
    
    features_pooled = torch.cat((features_mean, features_std, features_max), dim=-1)
    
    # Handle batch/channel dimensions as you did previously
    if features_pooled.dim() == 3:
        features_pooled = features_pooled.squeeze(1)
        
    return features_pooled